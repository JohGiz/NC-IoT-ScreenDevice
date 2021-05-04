# Nordcloud AWS IoT example

## Step by step
1. Create AWS IoT Thing
```
aws iot create-thing --thing-name NCHackDevice
```
Save the return value that looks like this
```
{
    "thingName": "NCHackDevice",
    "thingArn": "arn:aws:iot:<region>:<acountid>:thing/NCHackDevice",
    "thingId": "xxxxxx-xxxx-xxxx-xxx-xxxxxxxxxxxx"
}
```

2. Create certificate (use gui or cli)
1. Activate certificate
1. Download certificates
1. Create certificate
```
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": "iot:Connect",
      "Resource": "arn:aws:iot:<region>:<accountid>:client/${iot:Connection.Thing.ThingName}"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Subscribe",
      "Resource": "arn:aws:iot:<region>:<accountid>:topicfilter/${iot:Connection.Thing.ThingName}/oled-*"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Receive",
      "Resource": "arn:aws:iot:<region>:<accountid>:topic/${iot:Connection.Thing.ThingName}/oled-*"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Publish",
      "Resource": "arn:aws:iot:<region>:<accountid>:topic/oled/alive"
    }
  ]
}
```
1. Connect policy to certificate
1. Convert certificates [How to convert](https://github.com/copercini/esp8266-aws_iot)
1. Save to /data/-folder of the sketch
1. Copy config.h_EXAMPLE to config.h
1. Edit and add correct values to config.
1. Install [ESP8266 filesystem uploader](https://github.com/esp8266/arduino-esp8266fs-plugin)
1. Upload file
1. Upload sketch