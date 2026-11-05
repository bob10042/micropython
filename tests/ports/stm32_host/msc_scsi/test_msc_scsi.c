// Host harness for ports/stm32/usbdev/class/src/usbd_msc_scsi.c (AUD-023).
// A fake logical unit keeps written blocks in a write-back cache, like the
// pyboard flash block device.  SCSI SYNCHRONIZE CACHE must not report success
// while the cache still holds unwritten data.
#include <stdio.h>
#include <string.h>
#include "usbd_msc_bot.h"
#include "usbd_msc_scsi.h"
#include "usbd_cdc_msc_hid.h"

static int fake_dirty;
static int fake_sync_calls;
static int fake_sync_result;

// USB low-level and BOT functions the SCSI layer calls; none do anything here.
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint16_t size) {
    return USBD_OK;
}
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint16_t size) {
    return USBD_OK;
}
const uint8_t USBD_MSC_Mode_Sense6_Data[4];
const uint8_t USBD_MSC_Mode_Sense10_Data[8];
void MSC_BOT_SendCSW(USBD_HandleTypeDef *pdev, uint8_t CSW_Status) {
}

static int8_t fake_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len) {
    fake_dirty = 1; // data only in the cache
    return 0;
}

#if HAVE_SYNC
static int8_t fake_Sync(uint8_t lun) {
    ++fake_sync_calls;
    if (fake_sync_result == 0) {
        fake_dirty = 0;
    }
    return fake_sync_result;
}
#endif

static USBD_StorageTypeDef fake_ops = {
    .Write = fake_Write,
    #if HAVE_SYNC
    .Sync = fake_Sync,
    #endif
};

static int failures;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); ++failures; } } while (0)

int main(void) {
    static usbd_cdc_msc_hid_state_t state;
    static USBD_HandleTypeDef pdev;
    pdev.pClassData = &state;
    USBD_MSC_BOT_HandleTypeDef *hmsc = &state.MSC_BOT_ClassData;
    hmsc->bdev_ops = &fake_ops;

    for (int i = 0; i < 2; ++i) {
        uint8_t cmd[16] = { i == 0 ? SCSI_SYNCHRONIZE_CACHE10 : SCSI_SYNCHRONIZE_CACHE16 };

        // Host wrote a block (it sits in the device cache), then asks for a sync.
        fake_Write(0, NULL, 0, 1);
        fake_sync_result = 0;
        hmsc->scsi_sense_head = hmsc->scsi_sense_tail = 0;
        int8_t ret = SCSI_ProcessCmd(&pdev, 0, cmd);
        CHECK(ret == 0);
        CHECK(fake_dirty == 0); // success only once the data is on the medium
        CHECK(hmsc->scsi_sense_head == hmsc->scsi_sense_tail);

        // The medium write fails: the command must fail with a WRITE FAULT sense.
        fake_Write(0, NULL, 0, 1);
        fake_sync_result = -1;
        ret = SCSI_ProcessCmd(&pdev, 0, cmd);
        CHECK(ret < 0);
        // REQUEST SENSE reports sense key in byte 2 and ASC in byte 12.
        uint8_t req_sense[16] = { SCSI_REQUEST_SENSE, 0, 0, 0, REQUEST_SENSE_DATA_LEN };
        CHECK(SCSI_ProcessCmd(&pdev, 0, req_sense) == 0);
        CHECK(hmsc->bot_data[2] == HARDWARE_ERROR);
        CHECK(hmsc->bot_data[12] == WRITE_FAULT);
    }

    printf("msc_scsi: sync calls %d, %s\n", fake_sync_calls, failures ? "FAILED" : "OK");
    return failures ? 1 : 0;
}
