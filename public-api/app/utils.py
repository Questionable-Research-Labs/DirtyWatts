from math import log10, floor

def chunks(lst, n):
    """Yield successive n-sized chunks from lst."""
    for i in range(0, len(lst), n):
        yield lst[i:i + n]

def round_sig(x, sig=2):
    return round(x, sig-int(floor(log10(abs(x))))-1)
