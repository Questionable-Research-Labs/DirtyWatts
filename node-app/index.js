const mqtt = require('mqtt')
const express = require('express')

const client  = mqtt.connect('mqtt://127.0.0.1')
const app = express()

app.use(express.json())



client.on('connect', function () {
    console.log("connected")
    // client.publish('zigbee2mqtt/socket_1/set', '{"state": "ON"}')
})

// app.get('/', function (req, res) {
//     res.send('Hello World')
// })

app.post('/socket_1', function (req, res) {
    res.send(req.body)
    console.log(req.body)
    client.publish('zigbee2mqtt/socket_1/set', JSON.stringify(req.body)) 
    console.log('command sent')
})

app.listen(3000)

// client.on('message', function (topic, message) {
//     // message is Buffer
//     console.log(message.toString())
//     client.end()
// })