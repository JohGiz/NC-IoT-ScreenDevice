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

2. Create certificate, take note of CertificateId (use GUI or CLI command).
```
aws iot create-keys-and-certificate \
    --set-as-active \
    --certificate-pem-outfile "mything.cert.pem" \
    --public-key-outfile "mything.public.key" \
    --private-key-outfile "mything.private.key"
```
1. Activate certificate (Included in CLI command above)
1. Download certificates (Included in CLI command above)
1. Modify policy.json to match your requirements
1. Create your policy
```
aws iot create-policy \
    --policy-name NCHackPolicy \
    --policy-document file://policy.json
```
1. Connect policy to certificate
```
aws iot attach-policy \
    --policy-name NCHackPolicy \
    --target "arn:aws:iot:<region>:<account-id>:cert/<certificate-id-noted-above>"
```
1. Copy config.h_EXAMPLE to config.h
1. Edit and add correct values to config.h
1. Install [ESP8266 filesystem uploader](https://github.com/esp8266/arduino-esp8266fs-plugin)
1. Upload files from Aurdino IDE: Tools > ESP8266 Sketch Data Upload

# Setup done

## Monitor ESP device conection

1. Open AWS GUI
1. Under AWS Core > Test subscribe to topic "esp/alive"
1. Boot your ESP with the code uploaded

## Publish message to Thing
1. Open AWS GUI
1. Under AWS Core > Test publish to topic "NCOledESP/oled-display"
1. Watch Serial monitor after message arrived. 
1. Publish the following message to make it show on display
```
[
    { 
        "type": "article", 
        "preheadline": "Breaking news",
        "headline": "Nordcloud",
        "body": "AWS IoT Hackathon",
        "delay": 2000
    },
    { 
        "type": "article", 
        "preheadline": "2021-05-03 14:02",
        "headline": "Slack message",
        "body": "xxxxxx",
        "delay": 5000
    }

]
```
1. Do your magic and build something to publish some cool information to your ESP