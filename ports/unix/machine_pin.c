/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MicroPython contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// GPIO support for Linux using GPIO character device (gpiochip)
// This provides machine.Pin functionality for Raspberry Pi and other Linux SBCs
// Requires Linux kernel 5.10+ for full GPIO v2 API support (pull up/down)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"
#include "extmod/virtpin.h"

// GPIO chip device (default for Raspberry Pi 5 is gpiochip4, Pi 4 is gpiochip0)
#ifndef MICROPY_HW_GPIO_CHIP
#define MICROPY_HW_GPIO_CHIP "/dev/gpiochip4"
#endif

#ifndef MICROPY_HW_NUM_PINS
#define MICROPY_HW_NUM_PINS 64
#endif

#define GPIO_MODE_IN  (0)
#define GPIO_MODE_OUT (1)
#define GPIO_PULL_NONE (0)
#define GPIO_PULL_UP   (1)
#define GPIO_PULL_DOWN (2)

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    uint32_t id;
    int fd;          // file descriptor for this pin's line
    uint8_t mode;
    uint8_t pull;
    uint8_t value;
} machine_pin_obj_t;

static machine_pin_obj_t *machine_pin_obj_all[MICROPY_HW_NUM_PINS] = {0};

static int gpio_chip_fd = -1;

#if MICROPY_PY_THREAD
#include "py/mpthread.h"
static mp_thread_mutex_t gpio_chip_mutex;
static bool mutex_initialized = false;
#endif

// Open the GPIO chip device (thread-safe)
static int open_gpio_chip(void) {
    #if MICROPY_PY_THREAD
    if (!mutex_initialized) {
        mp_thread_mutex_init(&gpio_chip_mutex);
        mutex_initialized = true;
    }
    mp_thread_mutex_lock(&gpio_chip_mutex, 1);
    #endif

    if (gpio_chip_fd < 0) {
        gpio_chip_fd = open(MICROPY_HW_GPIO_CHIP, O_RDWR);
        if (gpio_chip_fd < 0) {
            // Try alternate chip for Raspberry Pi 4 and earlier
            gpio_chip_fd = open("/dev/gpiochip0", O_RDWR);
            if (gpio_chip_fd < 0) {
                // Try gpiochip1 for some boards
                gpio_chip_fd = open("/dev/gpiochip1", O_RDWR);
            }
        }
    }
    int fd = gpio_chip_fd;

    #if MICROPY_PY_THREAD
    mp_thread_mutex_unlock(&gpio_chip_mutex);
    #endif

    return fd;
}

// Cleanup function called on exit
void machine_pin_deinit_all(void) {
    for (int i = 0; i < MICROPY_HW_NUM_PINS; i++) {
        if (machine_pin_obj_all[i] && machine_pin_obj_all[i]->fd >= 0) {
            close(machine_pin_obj_all[i]->fd);
            machine_pin_obj_all[i]->fd = -1;
        }
    }
    if (gpio_chip_fd >= 0) {
        close(gpio_chip_fd);
        gpio_chip_fd = -1;
    }
}

static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "Pin(%u, mode=%s, pull=%s, value=%u)",
        self->id,
        self->mode == GPIO_MODE_IN ? "IN" : "OUT",
        self->pull == GPIO_PULL_UP ? "PULL_UP" :
            (self->pull == GPIO_PULL_DOWN ? "PULL_DOWN" : "NONE"),
        self->value);
}

static mp_obj_t machine_pin_obj_init_helper(machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_pull, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_pull, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_mode].u_obj != mp_const_none) {
        self->mode = mp_obj_get_int(args[ARG_mode].u_obj);
    }

    if (args[ARG_pull].u_obj != mp_const_none) {
        self->pull = mp_obj_get_int(args[ARG_pull].u_obj);
    }

    // Request the GPIO line from the kernel
    int chip_fd = open_gpio_chip();
    if (chip_fd < 0) {
        mp_raise_msg(&mp_type_OSError,
            MP_ERROR_TEXT("Cannot access GPIO. Run with sudo or add user to 'gpio' group"));
    }

    // Close existing line if already open
    if (self->fd >= 0) {
        close(self->fd);
        self->fd = -1;
    }

    // Try GPIO v2 API first (kernel 5.10+, supports pull up/down)
    #ifdef GPIO_V2_GET_LINE_IOCTL
    struct gpio_v2_line_request req_v2;
    memset(&req_v2, 0, sizeof(req_v2));
    req_v2.offsets[0] = self->id;
    req_v2.num_lines = 1;

    // Configure pull up/down
    if (self->pull == GPIO_PULL_UP) {
        req_v2.config.flags |= GPIO_V2_LINE_FLAG_BIAS_PULL_UP;
    } else if (self->pull == GPIO_PULL_DOWN) {
        req_v2.config.flags |= GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN;
    } else {
        req_v2.config.flags |= GPIO_V2_LINE_FLAG_BIAS_DISABLED;
    }

    // Configure direction
    if (self->mode == GPIO_MODE_IN) {
        req_v2.config.flags |= GPIO_V2_LINE_FLAG_INPUT;
    } else {
        req_v2.config.flags |= GPIO_V2_LINE_FLAG_OUTPUT;
        if (args[ARG_value].u_obj != mp_const_none) {
            self->value = mp_obj_is_true(args[ARG_value].u_obj);
            req_v2.config.values = (1ULL << 0);  // Set bit for default value
            if (!self->value) {
                req_v2.config.values = 0;
            }
        }
    }

    snprintf(req_v2.consumer, sizeof(req_v2.consumer), "micropython");

    if (ioctl(chip_fd, GPIO_V2_GET_LINE_IOCTL, &req_v2) == 0) {
        self->fd = req_v2.fd;
        return mp_const_none;
    }
    #endif

    // Fallback to GPIO v1 API (older kernels, no pull up/down support)
    struct gpiohandle_request req;
    memset(&req, 0, sizeof(req));
    req.lineoffsets[0] = self->id;
    req.lines = 1;

    if (self->mode == GPIO_MODE_IN) {
        req.flags = GPIOHANDLE_REQUEST_INPUT;
    } else {
        req.flags = GPIOHANDLE_REQUEST_OUTPUT;
        if (args[ARG_value].u_obj != mp_const_none) {
            self->value = mp_obj_is_true(args[ARG_value].u_obj);
            req.default_values[0] = self->value;
        }
    }

    snprintf(req.consumer_label, sizeof(req.consumer_label), "micropython");

    if (ioctl(chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &req) < 0) {
        mp_raise_OSError(errno);
    }

    self->fd = req.fd;

    // Warn if pull up/down was requested but not supported
    if (self->pull != GPIO_PULL_NONE) {
        mp_warning(NULL, "Pull up/down not supported on this kernel (need 5.10+)");
    }

    return mp_const_none;
}

static mp_obj_t machine_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Get pin id
    int pin_id = mp_obj_get_int(args[0]);
    if (pin_id < 0 || pin_id >= MICROPY_HW_NUM_PINS) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
    }

    // Get or create pin object
    machine_pin_obj_t *self = machine_pin_obj_all[pin_id];
    if (self == NULL) {
        self = mp_obj_malloc(machine_pin_obj_t, &machine_pin_type);
        self->id = pin_id;
        self->fd = -1;
        self->mode = GPIO_MODE_IN;
        self->pull = GPIO_PULL_NONE;
        self->value = 0;
        machine_pin_obj_all[pin_id] = self;
    }

    if (n_args > 1 || n_kw > 0) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    return MP_OBJ_FROM_PTR(self);
}

// Deinitialize pin and release resources
static mp_obj_t machine_pin_deinit(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->fd >= 0) {
        close(self->fd);
        self->fd = -1;
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_deinit_obj, machine_pin_deinit);

static mp_obj_t machine_pin_obj_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->fd < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("pin not initialized"));
    }

    if (n_args == 0) {
        // Get pin value
        struct gpiohandle_data data;
        if (ioctl(self->fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) < 0) {
            mp_raise_OSError(errno);
        }
        self->value = data.values[0];
        return MP_OBJ_NEW_SMALL_INT(self->value);
    } else {
        // Set pin value
        self->value = mp_obj_is_true(args[0]);
        struct gpiohandle_data data;
        data.values[0] = self->value;
        if (ioctl(self->fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
            mp_raise_OSError(errno);
        }
        return mp_const_none;
    }
}

static mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_obj_call(args[0], n_args - 1, 0, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

static mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->fd < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("pin not initialized"));
    }
    self->value = 1;
    struct gpiohandle_data data;
    data.values[0] = 1;
    if (ioctl(self->fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
        mp_raise_OSError(errno);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

static mp_obj_t machine_pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->fd < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("pin not initialized"));
    }
    self->value = 0;
    struct gpiohandle_data data;
    data.values[0] = 0;
    if (ioctl(self->fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
        mp_raise_OSError(errno);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

static mp_obj_t machine_pin_toggle(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->fd < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("pin not initialized"));
    }

    // Read current value
    struct gpiohandle_data data;
    if (ioctl(self->fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) < 0) {
        mp_raise_OSError(errno);
    }

    // Toggle it
    self->value = !data.values[0];
    data.values[0] = self->value;

    if (ioctl(self->fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
        mp_raise_OSError(errno);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_toggle_obj, machine_pin_toggle);

// Get current pin mode
static mp_obj_t machine_pin_mode(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->mode);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_mode_obj, machine_pin_mode);

// Get current pin pull configuration
static mp_obj_t machine_pin_pull(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->pull);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_pull_obj, machine_pin_pull);

static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // Instance methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_pin_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&machine_pin_toggle_obj) },
    { MP_ROM_QSTR(MP_QSTR_mode), MP_ROM_PTR(&machine_pin_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_pull), MP_ROM_PTR(&machine_pin_pull_obj) },

    // Class constants
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(GPIO_MODE_IN) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(GPIO_MODE_OUT) },
    { MP_ROM_QSTR(MP_QSTR_PULL_NONE), MP_ROM_INT(GPIO_PULL_NONE) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP), MP_ROM_INT(GPIO_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(GPIO_PULL_DOWN) },
};
static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

static mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->fd < 0) {
        *errcode = MP_EINVAL;
        return -1;
    }

    switch (request) {
        case MP_PIN_READ: {
            struct gpiohandle_data data;
            if (ioctl(self->fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) < 0) {
                *errcode = errno;
                return -1;
            }
            return data.values[0];
        }
        case MP_PIN_WRITE: {
            struct gpiohandle_data data;
            data.values[0] = arg;
            if (ioctl(self->fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
                *errcode = errno;
                return -1;
            }
            return 0;
        }
    }
    *errcode = MP_EINVAL;
    return -1;
}

static const mp_pin_p_t pin_pin_p = {
    .ioctl = pin_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    make_new, machine_pin_make_new,
    print, machine_pin_print,
    call, machine_pin_obj_call,
    protocol, &pin_pin_p,
    locals_dict, &machine_pin_locals_dict
);
