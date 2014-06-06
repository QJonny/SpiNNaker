// simulation constants
#define MS 1000
#define TICK_TIME (100*MS) // in ms
#define INV_TICK_TIME 10
// end of simulation constants

// ball tracking
uint x_buffer[16] = {0};
uint y_buffer[16] = {0};
uint total_sum_x = 0;
uint total_sum_y = 0;
uint curr_index = 0;
// end of ball tracking

// ball parameters
uint x_pos = 0;
uint y_pos = 0;
uint old_x_pos = 0;
uint old_y_pos = 0;
int x_speed = 0;
int y_speed = 0;
int old_x_speed = 0;
int old_y_speed = 0;
int x_acceleration = 0;
int y_acceleration = 0;
// end of ball parameters


void compute_pos(uint x_cur, uint y_cur)
{
        total_sum_x = total_sum_x - x_buffer[curr_index] + x_cur;
        x_buffer[curr_index] = x_cur;
        total_sum_y = total_sum_y - y_buffer[curr_index] + y_cur;
        y_buffer[curr_index] = y_cur;

        x_pos = total_sum_x >> 4; // division by 16
        y_pos = total_sum_y >> 4; // division by 16


        curr_index = (curr_index + 1) % 16;
}

// call every tick (0.1s)
void compute_speed()
{
        x_speed = ((int)x_pos - (int)old_x_pos) * INV_TICK_TIME;
        y_speed = ((int)y_pos - (int)old_y_pos) * INV_TICK_TIME;


        old_x_pos = x_pos;
        old_y_pos = y_pos;
}

// call every tick (0.1s)
void compute_acceleration()
{
        x_acceleration = (x_speed - old_x_speed) * INV_TICK_TIME;
        y_acceleration = (y_speed - old_y_speed) * INV_TICK_TIME;

        old_x_speed = x_speed;
        old_y_speed = y_speed;
}

