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
            let power_types = data.power_types

            // pseudo data (comment/uncomment during testing)
            power_types.battery.generation_mw       = 0        //max 1
            power_types.co_gen.generation_mw        = 118        //max 216
            power_types.coal.generation_mw          = 0        //max 750
            power_types.gas.generation_mw           = 197        //max 1280
            power_types.geothermal.generation_mw    = 933        //max 1062
            power_types.hydro.generation_mw         = 2950        //max 5415
            power_types.diesel.generation_mw        = 0        //max 156
            power_types.wind.generation_mw          = 629        //max 1040


            let total_generation_mw = 0   // total up power usage
            for(key in power_types){
                let power_type = power_types[key];
                total_generation_mw += power_type.generation_mw;
            }


            console.log("====================")
            console.log("Total Power Generation: " + total_generation_mw + " MW")

            let bad_generation_mw = 0   // total up non-renewables power usage
            bad_generation_mw = power_types.coal.generation_mw + power_types.gas.generation_mw + power_types.diesel.generation_mw // + power_types.co_gen.generation_mw
            console.log("Non-Renewable Power: " + bad_generation_mw + " MW")
            
            percent_fossil = 100 * (bad_generation_mw / total_generation_mw)  // calculate % non-renewable
            console.log("Percentage Non-Renewable: " + percent_fossil + "%")

            // COAL = 1073 grams co2 per kWh
            // GAS = 410 grams co2 per kWh
            // COGEN = 249 grams co2 per kWh
            // GEO = 90 grams co2 per kWh

            // WEIGHTED PERCENTAGE
            let weighted_bad_generation = 0
            weighted_bad_generation = power_types.coal.generation_mw * 10.73 + power_types.gas.generation_mw * 4.10 + power_types.co_gen.generation_mw * 2.49
            console.log("Weighted Bad Generation: " + weighted_bad_generation)


            // UPDATE LIGHT
            let red_value = 0
            let green_value = 0

            // 1000
            
            let fully_red = 1500
            let fully_green = 1000 // eg 100
            fully_red -= fully_green // now 1400

            weighted_bad_generation -= fully_green // 900


            weighted_light_value = weighted_bad_generation / fully_red // 0.64

            // console.log(weighted_light_value, weighted_bad_generation, fully_green, fully_red)

            if(weighted_light_value < 0){ //make sure not cringe
                weighted_light_value = 0
            }
            
            if(weighted_light_value > 1){ //make sure not cringe part 2
                weighted_light_value = 1
            }

            red_value = Math.round(255 * weighted_light_value)
            green_value = Math.round(255 - (255 * weighted_light_value))



        

            // if(percent_fossil > 10){
            //     percent_fossil = 10
            // }
            // let fossil_amount = 0 
            // fossil_amount = percent_fossil / 10
            // console.log("Fossil: " + fossil_amount + " (% / 10)")
            // 
            // red_value = Math.round(255 * fossil_amount)
            // green_value = Math.round(255 - (255 * fossil_amount))

            console.log("Value For Lamp: (" + red_value + ", " + green_value + ", 0) (RGB)")
            
            let lamp_info = {"state": "ON", "color": {"r": red_value, "g": green_value, "b": 0}}

            client.publish('zigbee2mqtt/lamp_rgb_1/set', JSON.stringify(lamp_info)) 


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