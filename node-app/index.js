const mqtt = require('mqtt')
const express = require('express')
const https = require('https')
const { response } = require('express')
const { waitForDebugger } = require('inspector')
const { resolveCaa } = require('dns')
const client  = mqtt.connect('mqtt://127.0.0.1')
const app = express()
app.use(express.json())


function getDataFromAPI() {
    return new Promise((resolve, reject) => {
        https.get('https://dirty-watts-api.host.qrl.nz/power_stations', (response) => {
            let data = ''
            response.on('data', (chunk) => {
                data += chunk
            })
    
            response.on('end', () => {
                // console.log(data)
                try {
                parsed_data = JSON.parse(data);
                console.log(JSON.stringify(parsed_data))
                resolve(parsed_data);
                } catch (e) {
                    reject(e)
                }
            })
        })
    
        .on('error', (error) => {
            console.log(error)
            reject();
        })
    });
}


function update() {
    getDataFromAPI()
        .then((data) => {
            let total_generation_mw = 0   // total up power usage
            let power_types = data.power_types
            for(key in power_types){
                let power_type = power_types[key];
                total_generation_mw += power_type.generation_mw;
            }
            console.log(total_generation_mw)

            let bad_generation_mw = 0   // total up non-renewables power usage
            bad_generation_mw = power_types.coal.generation_mw + power_types.gas.generation_mw + power_types.liquid.generation_mw
            console.log(bad_generation_mw)
            
            percent_fossil = bad_generation_mw / total_generation_mw  // calculate % non-renewable
            console.log(percent_fossil)


            // UPDATE LIGHT
            client.publish('zigbee2mqtt/lamp_rgb_1/set', '{"state": "ON", "color": {"r": 255, "g": 255, "b": 0}}') 


        }).catch(console.error)
}

setInterval(() => {
    update()
}, 1000 * 60)

update()

// client.on('connect', function () {
//     console.log("connected")
//     // client.publish('zigbee2mqtt/socket_1/set', '{"state": "OFF"}')  
// })

// app.post('/socket_1', function (req, res) {
//     res.send(req.body)
//     console.log(JSON.stringify(req.body))
//     client.publish('zigbee2mqtt/socket_1/set', JSON.stringify(req.body)) 
//     console.log('command sent')
// })

// app.post('/lamp_rgb_1', function (req, res) {
//     res.send(req.body)
//     console.log(JSON.stringify(req.body))
//     client.publish('zigbee2mqtt/lamp_rgb_1/set', JSON.stringify(req.body)) 
//     console.log('command sent')
// })

// app.listen(3000)

// client.on('message', function (topic, message) {
//     // message is Buffer
//     console.log(message.toString())
//     client.end()
// })