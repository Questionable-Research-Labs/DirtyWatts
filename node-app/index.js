const mqtt = require('mqtt')
const client  = mqtt.connect('127.0.0.1')

client.on('connect', function () {
    console.log("connected")
    client.publish('zigbee2mqtt/socket_1/set', '{"state": "ON"}')
})

// client.on('message', function (topic, message) {
//     // message is Buffer
//     console.log(message.toString())
//     client.end()
// })