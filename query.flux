import "math"
from(bucket: "Dirty Watts")
  |> range(start: v.timeRangeStart, stop: v.timeRangeStop)
  |> filter(fn: (r) => r["_measurement"] == "emi_stat")
  |> pivot(rowKey: ["_time"], columnKey: ["_field"], valueColumn: "_value")
  |> map(
        fn: (r) => ({r with
            lat: math.round(x: float(v: r.lat) * 100.0) / 100.0,
            lng: math.round(x: float(v: r.lng) * 100.0) / 100.0
        }),
     )
  |> group(columns: ["lat", "lng"])
