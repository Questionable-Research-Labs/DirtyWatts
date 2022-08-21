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
            let outlet_state = true

            // pseudo data (comment/uncomment during testing)
            // power_types.battery.generation_mw       = 0        //max 1
            // power_types.co_gen.generation_mw        = 118        //max 216
            // power_types.coal.generation_mw          = 10        //max 750
            // power_types.gas.generation_mw           = 197        //max 1280
            // power_types.geothermal.generation_mw    = 933        //max 1062
            // power_types.hydro.generation_mw         = 2950        //max 5415
            // power_types.diesel.generation_mw        = 0        //max 156
            // power_types.wind.generation_mw          = 629        //max 1040


            // let total_generation_mw = 0   // total up power usage
            // for(key in power_types){
            //     let power_type = power_types[key];
            //     total_generation_mw += power_type.generation_mw;
            // }

            // console.log("====================")
            // console.log("Total Power Generation: " + total_generation_mw + " MW")

            // let bad_generation_mw = 0   // total up non-renewables power usage
            // bad_generation_mw = power_types.coal.generation_mw + power_types.gas.generation_mw + power_types.diesel.generation_mw // + power_types.co_gen.generation_mw
            // console.log("Non-Renewable Power: " + bad_generation_mw + " MW")
            
            // percent_fossil = 100 * (bad_generation_mw / total_generation_mw)  // calculate % non-renewable
            // console.log("Percentage Non-Renewable: " + percent_fossil + "%")


            // // UPDATE LIGHT
            // let red_value = 0
            // let green_value = 0

            // if (power_types.coal.generation_mw > 0 || power_types.diesel.generation_mw > 0){
            //     let half_max_coal = Math.round(power_types.coal.capacity_mw/2 + power_types.diesel.capacity_mw/2)
            //     let bad_generation = (power_types.coal.generation_mw + power_types.diesel.generation_mw) / half_max_coal
            //     if(bad_generation > 1){
            //         bad_generation = 1
            //     }
                
            //     red_value = 150 + Math.round(105 * bad_generation)
            //     green_value = Math.round(95 - (95 * bad_generation))

            //     outlet_state = false
            // } else {
            //     let half_max_gas = Math.round(power_types.gas.capacity_mw/2)
            //     let medium_generation = power_types.gas.generation_mw / half_max_gas
            //     if(medium_generation > 1){
            //         medium_generation = 1
            //     }

            //     red_value = Math.round(160 * medium_generation)
            //     green_value = Math.round(255 - (127 * medium_generation))

            //     outlet_state = true
            // }



            // console.log("Value For Lamp: (" + red_value + ", " + green_value + ", 0) (RGB)")
            // let lamp_info = {"state": "ON", "color": {"r": red_value, "g": green_value, "b": 0}}
            // let switch_info = {"state": outlet_state}

            // client.publish('zigbee2mqtt/lamp_rgb_1/set', JSON.stringify(lamp_info)) 
            // client.publish('zigbee2mqtt/socket_1/set', JSON.stringify(switch_info))


        }).catch(console.error)
}

setInterval(() => {
    update()
}, 1000 * 0.5)

update()

function fancyLights() {

    let red = 0;
    let green = 255;

    setInterval(() => {

        console.log("Value For Lamp: (" + red + ", " + green + ", 0) (RGB)")
        let lamp_info = {"state": "ON", "color": {"r": red, "g": green, "b": 0}}
        let switch_info = {"state": true}

        client.publish('zigbee2mqtt/lamp_rgb_1/set', JSON.stringify(lamp_info)) 
        client.publish('zigbee2mqtt/socket_1/set', JSON.stringify(switch_info))

        if(red < 255) red++;
        if(green > 0) green--;
    }, 100)


}

fancyLights();

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