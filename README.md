# TSD305Lib-Arduino
Library for interfacing TSD305 IR thermopile sensor with Arduino compatible boards.

## The library provides following features
- Object Temperature
- Sensor Temperature (Ambient Temperature)
- Method for conversion from °C to °F

### Following public methods are provided:

- `tsd_eeprom_struct begin(bool debug);`
- `bool isConnected(void);`
- `float getSensorTemp(void);`
- `float getObjectTemp(void);`
- `float DtoF(float temp);`

*Following structure can be used to determine the hardware measurement limits of the varient of TSD305 sensor you are using:*
```
struct tsd_eeprom_struct {
  int16_t amb_min;
  int16_t amb_max;
  int16_t obj_min;
  int16_t obj_max;
  int16_t adc_cal;
};
```

### Example
Refer the example by clicking [here](https://github.com/pranjal-joshi/TSD305Lib-Arduino/blob/master/examples/tsd305_demo/tsd305_demo.ino) to get started.

### Author
* **Pranjal Joshi** (Deputy Engineer, [Bharat Electronics Limited](http://www.bel-india.in/), Pune)

### License
This project is licensed under the MIT License - see the [LICENSE.md](https://github.com/pranjal-joshi/TSD305Lib-Arduino/blob/master/LICENSE) file for details

### Contact
E-mail: pranjaljoshi@bel.co.in or joshi.pranjal5@gmail.com
Pull request for bug-fixes and new features are welcomed.

### Acknowledgements
* [Arduino](http://arduino.cc/)
