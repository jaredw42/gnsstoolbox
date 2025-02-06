# gnsstoolbox
simple tools for manipulating GNSS data


## timecalc

### installing timecalc
timecalc is now available on Pip.
```
pip install gnsstoolbox-timecalc
```
### running timecalc
If installed using Pip, timecalc can be run from anywhere using `gnsstimecalc` 

```
gnsstimecalc -h
usage: timecalc.py [-h] [--utc_offset UTC_OFFSET] 
[--datetime DATETIME | --epoch EPOCH | --epoch_sec EPOCH_SEC | --now NOW]

options:
  -h, --help            show this help message and exit
  --utc_offset UTC_OFFSET, -uo UTC_OFFSET
                        UTC offset in hours (default 0)
  --datetime DATETIME, -dt DATETIME
                        Datetime in format YYYY-MM-DD H:M:S
  --epoch EPOCH, --epoch_ms EPOCH, -em EPOCH
                        Unix epoch timestamp in milliseconds
  --epoch_sec EPOCH_SEC, -es EPOCH_SEC
                        Unix epoch timestamp in seconds
  --now NOW, -n NOW     The current device time
```