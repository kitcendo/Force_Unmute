A simple C++ program
to fix the issue where Windows mutes certain audio devices after sleep/hibernate/display off.

**Functions:**

1. Wait 10 seconds to allow Windows 11 to fully reconnect all audio devices.
2. Retrieve the names of all audio devices.
3. Apply everything necessary to unmute every device.
4. Log the status of each device, checking which ones are still muted.
5. Loop through all functions until every device is unmuted.
6. Once all devices are unmuted, display the status of each device for 10 seconds, then close the window.

Just run the EXE file to use it.
It is recommended to run it in **Task Scheduler** when the machine starts.
