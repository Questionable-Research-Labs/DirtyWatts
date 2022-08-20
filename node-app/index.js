const mqtt = require('mqtt')
const express = require('express')
const https = require('https')
const { response } = require('express')
const { waitForDebugger } = require('inspector')
const { resolveCaa } = require('dns')
const { fips } = require('crypto')
const client  = mqtt.connect('mqtt://127.0.0.1')
const app = express()
app.use(express.json())


function getDataFromAPI() {
    return new Promise((resolve, reject) => {
        https.get('https://dirty-watts-api.host.qrl.nz/live/power_stations', (response) => {
            let data = ''
            response.on('data', (chunk) => {
                data += chunk
            })
    
            response.on('end', () => {
                // console.log(data)
                try {
                parsed_data = JSON.parse(data);
                //console.log(JSON.stringify(parsed_data))
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

            console.log("====================")
            console.log("Total Power Generation: " + total_generation_mw + " MW")

            let bad_generation_mw = 0   // total up non-renewables power usage
            bad_generation_mw = power_types.coal.generation_mw + power_types.gas.generation_mw + power_types.diesel.generation_mw
            console.log("Non-Renewable Power: " + bad_generation_mw + " MW")
            
            percent_fossil = 100 * (bad_generation_mw / total_generation_mw)  // calculate % non-renewable
            console.log("Percentage Non-Renewable: " + percent_fossil + "%")

            // COAL = 1073 grams co2 per kWh
            // GAS = 410 grams co2 per kWh
            // COGEN = 249 grams co2 per kWh
            // GEO = 90 grams co2 per kWh

            // WEIGHTED PERCENTAGE
            let weighted_bad_generation = 0
            weighted_bad_generation = power_types.coal.generation_mw * 10.73 + power_types.gas.generation_mw * 4.10 + power_types.geothermal.generation_mw * 0.90 + power_types.co_gen.generation_mw * 2.49
            console.log("Weighted Bad Generation: " + weighted_bad_generation)


            // UPDATE LIGHT

            if(percent_fossil > 20){
                percent_fossil = 20
            }
            let fossil_amount = 0 
            fossil_amount = percent_fossil / 20
            console.log("Fossil: " + fossil_amount + " (% / 20)")
            let red_value = 0
            let green_value = 0
            red_value = Math.round(255 * fossil_amount)
            green_value = Math.round(255 - (255 * fossil_amount))
            console.log("Value For Lamp: " + red_value + " " + green_value, + " " + 0 + " (RGB)")
            
            let lamp_info = {"state": "ON", "color": {"r": red_value, "g": green_value, "b": 0}}

            client.publish('zigbee2mqtt/lamp_rgb_1/set', lamp_info) 


        }).catch(console.error)
}

setInterval(() => {
    update()
}, 1000 * 5)

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