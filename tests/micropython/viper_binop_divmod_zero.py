# test that viper int // and % by zero raise ZeroDivisionError (AUD-027)
# requires: viper


@micropython.viper
def div(x: int, y: int) -> int:
    return x // y


@micropython.viper
def mod(x: int, y: int) -> int:
    return x % y


for f in (div, mod):
    for x in (7, -7, 0):
        try:
            print(f(x, 0))
        except ZeroDivisionError:
            print("ZeroDivisionError")

# non-zero divisors are unchanged
print(div(7, 2), div(-7, 2), mod(7, 2), mod(-7, 2), mod(7, -2))
