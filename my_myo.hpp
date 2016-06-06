#include <myo/libmyo.h>
#include <vec/vec.hpp>

struct myo_data
{
    struct usr_dat
    {
        quat qrot;
        vec4f acc;
    } dat;

    bool is_initialised = false;

    libmyo_hub_t hub;

    myo_data()
    {
        libmyo_error_details_t err;

        libmyo_result_t res = libmyo_init_hub(&hub, nullptr, &err);

        if(res != libmyo_success)
        {
            const char* str = libmyo_error_cstring(err);

            printf("Error %s %i\n", str, res);

            libmyo_free_error_details(err);
        }
        else
        {
            is_initialised = true;
        }
    }


    static libmyo_handler_result_t handler(void* user_data, libmyo_event_t event)
    {
        uint32_t event_type = libmyo_event_get_type(event);

        if(event_type == libmyo_event_orientation)
        {
            float x = libmyo_event_get_orientation(event, libmyo_orientation_x);
            float y = libmyo_event_get_orientation(event, libmyo_orientation_y);
            float z = libmyo_event_get_orientation(event, libmyo_orientation_z);
            float w = libmyo_event_get_orientation(event, libmyo_orientation_w);

            usr_dat* dat = (usr_dat*)user_data;

            quat q;
            q.from_vec({x, y, z, w});

            dat->qrot = q;
        }

        return libmyo_handler_stop;
    }

    void tick()
    {
        if(!is_initialised)
            return;

        libmyo_error_details_t err;

        libmyo_result_t res = libmyo_run(hub, 100, handler, &dat, &err);

        if(res != libmyo_success)
        {
            printf("Error %s\n", libmyo_error_cstring(err));

            libmyo_free_error_details(err);
        }
    }

    void debug()
    {
        if(!is_initialised)
            return;

        printf("q %f %f %f %f\n", dat.qrot.q.v[0], dat.qrot.q.v[1], dat.qrot.q.v[2], dat.qrot.q.v[3]);
    }

    ~myo_data()
    {
        if(!is_initialised)
            return;

        libmyo_error_details_t err;

        libmyo_result_t res = libmyo_shutdown_hub(hub, &err);

        if(res != libmyo_success)
        {
            printf("Error res %i\n", res);

            libmyo_free_error_details(err);
        }
    }
};
