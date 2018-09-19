#ifndef DEFINITION_H
#define DEFINITION_H

#include <stdint.h>

#define BC_PC_ID                        0x01
#define BC_BOARD_ID                     0x02

#define BC_GET                          0x00
#define BC_SET                          0x80

#define BC_REALTIME_STATUS              0x01
#define BC_CONFIG                       0x02



#define BC_MODEL_LENGTH                 8
#define BC_DATA_POINT_LENGTH            16


typedef union Float {
        int8_t buffer[4];
        float num;
    }Float;

    typedef union Double {
        int8_t buffer[8];
        double num;
    }Double;


class Convert {
public:

    static double arrayToDouble(const uint8_t *data) {
        Double number;
        number.buffer[0] = data[0];
        number.buffer[1] = data[1];
        number.buffer[2] = data[2];
        number.buffer[3] = data[3];
        number.buffer[4] = data[4];
        number.buffer[5] = data[5];
        number.buffer[6] = data[6];
        number.buffer[7] = data[7];

        return number.num;
    }
    static void doubleToArray(double value, uint8_t *data) {
        Double number;
        number.num = value;
        data[0] = number.buffer[0];
        data[1] = number.buffer[1];
        data[2] = number.buffer[2];
        data[3] = number.buffer[3];
        data[4] = number.buffer[4];
        data[5] = number.buffer[5];
        data[6] = number.buffer[6];
        data[7] = number.buffer[7];
    }
    static void arrayToFloat(const uint8_t *data) {
        (void)data;
    }
    static void floatToArray(float value, uint8_t *data) {
        (void)value;
        (void)data;
    }

};

#endif // DEFINITION_H
