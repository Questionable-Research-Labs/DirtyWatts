const mqtt = require('mqtt')
const express = require('express')
import fetch from 'node-fetch';

const client  = mqtt.connect('mqtt://127.0.0.1')
const app = express()

app.use(express.json())



client.on('connect', function () {
    console.log("connected")
    // client.publish('zigbee2mqtt/socket_1/set', '{"state": "OFF"}')
    const response = await fetch('https://dirty-watts-api.host.qrl.nz/power_stations');
    const body = await response.json();

    console.log(body);
})

app.post('/socket_1', function (req, res) {
    res.send(req.body)
    console.log(JSON.stringify(req.body))
    client.publish('zigbee2mqtt/socket_1/set', JSON.stringify(req.body)) 
    console.log('command sent')
})

app.post('/lamp_rgb_1', function (req, res) {
    res.send(req.body)
    console.log(JSON.stringify(req.body))
    client.publish('zigbee2mqtt/lamp_rgb_1/set', JSON.stringify(req.body)) 
    console.log('command sent')
})


app.listen(3000)

// client.on('message', function (topic, message) {
//     // message is Buffer
//     console.log(message.toString())
//     client.end()
// })