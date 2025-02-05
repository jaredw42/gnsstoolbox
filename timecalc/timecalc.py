#!/usr/bin/env python3
import argparse
from datetime import datetime, timedelta, timezone
import logging


# constants
CALENDAR_DAY_FORMAT = "'%Y-%m-%d'"
DATETIME_WITH_UTC_OFFSET_FORMAT = "%Y-%m-%d %H:%M:%S%z"
DATETIME_FORMAT = "%Y-%m-%d %H:%M:%S"

logger = logging.getLogger()
logging.basicConfig(level=logging.DEBUG, format="Timecalc: %(message)s")


class GpsTime:
    """
    Container and methods for calculating the GPS system time
    """

    # constants
    GPS_EPOCH = "1980-01-06 00:00:00+00:00"
    MICROSEC_TO_SEC = 1e-6
    MILLISEC_TO_SEC = 1e-3
    DAYS_IN_WEEK = 7
    SECONDS_IN_DAY = 86400

    # dict containing leap second values using dates as keys
    # Leap second calendar taken from this website:
    # https://www.ptb.de/cms/en/ptb/fachabteilungen/abt4/fb-44/ag-441/realisation-of-legal-time-in-germany/leap-seconds.html
    LEAP_SECONDS = {
        "2017-01-01 00:00:00+00:00": 18,
        "2015-07-01 00:00:00+00:00": 17,
        "2012-07-01 00:00:00+00:00": 16,
        "2009-01-01 00:00:00+00:00": 15,
        "2006-01-01 00:00:00+00:00": 14,
        "1999-01-01 00:00:00+00:00": 13,
        "1997-07-01 00:00:00+00:00": 12,
        "1996-01-01 00:00:00+00:00": 11,
        "1994-07-01 00:00:00+00:00": 10,
        "1993-07-01 00:00:00+00:00": 9,
        "1992-07-01 00:00:00+00:00": 8,
        "1991-01-01 00:00:00+00:00": 7,
        "1990-01-01 00:00:00+00:00": 6,
        "1988-01-01 00:00:00+00:00": 5,
        "1985-07-01 00:00:00+00:00": 4,
        "1983-07-01 00:00:00+00:00": 3,
        "1982-07-01 00:00:00+00:00": 2,
        "1981-07-01 00:00:00+00:00": 1,
        GPS_EPOCH: 0,
    }

    def __init__(self, utc):

        self.gpstimefromutc(utc)

    def gpstimefromutc(self, utc):
        """
        Find the given leap second for a UTC datetime,
        then calculate the GPS week number and seconds into week
        """

        self.leap_seconds = self.get_leap_seconds(utc)

        gps_epoch = datetime.strptime(self.GPS_EPOCH, DATETIME_WITH_UTC_OFFSET_FORMAT)
        tdiff = utc - gps_epoch + timedelta(seconds=self.leap_seconds)
        self.gps_week = tdiff.days // self.DAYS_IN_WEEK
        days_into_week = tdiff.days - (self.gps_week * self.DAYS_IN_WEEK)
        self.gps_seconds_into_week = (
            tdiff.seconds
            + (self.SECONDS_IN_DAY * days_into_week)
            + (tdiff.microseconds * self.MICROSEC_TO_SEC)
        )

    def get_leap_seconds(self, utc):
        """
        Get the number of leap seconds for a given datetime
        """
        ls_datetimes = sorted(
            [
                datetime.strptime(x, DATETIME_WITH_UTC_OFFSET_FORMAT)
                for x in self.LEAP_SECONDS.keys()
            ]
        )
        # check if dt is after the last added leap second
        if utc > ls_datetimes[-1]:
            leapsec = self.LEAP_SECONDS[(str(ls_datetimes[-1]))]
            logger.info(
                f"Date {datetime.strftime(utc, '%Y-%m-%d')} exceeds last leap second addition date of {ls_datetimes[-1]}. Leap seconds set to {leapsec}"
            )
            return leapsec
        # check if dt is before GPS epoch
        elif utc < ls_datetimes[0]:
            logger.info(
                f"Date {datetime.strftime(utc, '%Y-%m-%d')} is before the GPS Epoch. No leap seconds applied."
            )
            return leapsec
        # iterate through the leap second dates and the find the pair dt is between.
        # the lower value is the number of leap seconds to apply
        prev_lsd = ls_datetimes[0]
        for lsd in ls_datetimes:
            if lsd > utc:
                leapsec = self.LEAP_SECONDS[str(prev_lsd)]
                logger.info(
                    f"There were {leapsec} leap seconds after {datetime.strftime(prev_lsd, '%Y-%m-%d')}"
                )
                return leapsec

            prev_lsd = lsd


def get_utc_time():
    """
    Parse the CLI args and create a UTC datetime object
    """
    parser = argparse.ArgumentParser()
    dt_group = parser.add_mutually_exclusive_group()
    parser.add_argument(
        "--utc_offset", "-uo", type=float, help="UTC offset in hours [default: 0]"
    )
    dt_group.add_argument(
        "--datetime",
        "-dt",
        type=str,
        help=f"Datetime in format YYYY-MM-DD H:M:S",
    )
    dt_group.add_argument(
        "--epoch",
        "--epoch_ms",
        "-em",
        type=int,
        help="Unix epoch timestamp in milliseconds",
    )
    dt_group.add_argument(
        "--epoch_sec", "-es", type=float, help="Unix epoch timestamp in seconds"
    )
    dt_group.add_argument("--now", "-n", type=bool, help="The current device time")

    args = parser.parse_args()

    dt = None

    if args.utc_offset:
        tz = timezone(offset=timedelta(hours=args.utc_offset))
    else:
        tz = timezone.utc

    if args.now:
        logger.info("Using device local time for datetime")
        dt = datetime.now(tz)

    elif args.epoch:
        logger.info(f"Using {args.epoch} as epoch seconds")
        dt = datetime.fromtimestamp(tz)

    elif args.datetime:
        # attempt to parse the datetime string, both with and without a UTC offset specifier.
        try:
            dt = datetime.strptime(args.datetime, DATETIME_WITH_UTC_OFFSET_FORMAT)
        except ValueError as e:
            try:
                dt = datetime.strptime(args.datetime, DATETIME_FORMAT)
                dt = dt.replace(tzinfo=tz)
            except ValueError as e:
                logger.error(e)
                raise ValueError("Could not parse datetime argument.")

    if not dt:
        logger.info("No datetime entered, using device local time")
        dt = datetime.now(tz)

    return dt


def timecalc():
    """
    converts UTC datetimes to GPS weeks and seconds
    accepts UTC epoch time or UTC datetime in format  "%Y-%m-%d %H:%M:%S"
    """

    logger.info("timecalc started.")

    dt = get_utc_time()
    gpstime = GpsTime(dt)
    logger.info(
        f"UTC DATETIME: {dt} \n          GPS WEEK: {gpstime.gps_week}, TOW: {gpstime.gps_seconds_into_week:.2f}"
    )


if __name__ == "__main__":

    timecalc()
