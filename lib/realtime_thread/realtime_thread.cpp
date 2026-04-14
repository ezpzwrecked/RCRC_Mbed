#include "realtime_thread.h"

#include <chrono>
#include <cstdint>

#include "DataLogger.h"
#include "GPA.h"

extern DataLogger myDataLogger;
extern GPA myGPA;

using namespace Eigen;
using namespace std::chrono;

// contructor for realtime_thread loop
realtime_thread::realtime_thread(IO_handler *io_handler, float Ts)
    : m_Thread(osPriorityHigh1, 1024)
{
    m_Ts = Ts;                 // the sampling time
    m_IO_handler = io_handler; // a pointer to the io handler
    m_Timer.reset();
    m_Timer.start();
}

// decontructor for controller loop
realtime_thread::~realtime_thread() {}

// this is the main loop called every Ts with high priority
void realtime_thread::loop(void)
{
    float time{0.0f}, w{0.0f}, y1{0.0f}, y2{0.0f}, u(0.0f), exc(0.0f);

    Matrix<float, 1, 2> K;
    K << -0.791f, 2.2308f;
    float V = 2.4398f;

    while (true) {
        ThisThread::flags_wait_any(m_ThreadFlag);
        const float time = 1e-6f * (float)(duration_cast<microseconds>(m_Timer.elapsed_time()).count());
        // --------------------- THE LOOP ---------------------

        float w = myDataLogger.get_set_value(time); // get set values from the GUI

        Vector2f x;
        x << m_IO_handler->read_ain1(), m_IO_handler->read_ain2(); // read 1st and 2nd voltage

        u = V * w - K * x; // state feedback control law


        /*u = myDataLogger.get_set_value(time); // get set values from the GUI

        const float y1 = m_IO_handler->read_ain1(); // read 1st voltage
        const float y2 = m_IO_handler->read_ain2(); // read 2nd voltage

        // m_IO_handler->write_aout(w); // write to analog output

        myDataLogger.write_to_log(time, u, y1, y2, 0.0f, 0.0f, 0.0f);
        */

        /*m_IO_handler->write_aout(u); // write to analog output
        u = myGPA.update(u, m_IO_handler->read_ain2()); // update the controller and get the new control value
        */

        /*
        w = myDataLogger.get_set_value(time); // get set values from the GUI
        float error = w - m_IO_handler->read_ain2(); // calculate error
        u = 4.0f * error; // simple P controller with gain 4.0
        */

        m_IO_handler->write_aout(u); // write to analog output

        myDataLogger.write_to_log(time, w, x(0), x(1), u, 0.0f, 0.0f); // log data

    }
}

float realtime_thread::saturate(float x, float ll, float ul)
{
    if (x > ul)
        return ul;
    else if (x < ll)
        return ll;
    return x;
}

void realtime_thread::sendSignal() { m_Thread.flags_set(m_ThreadFlag); }

void realtime_thread::start_loop(void)
{
    m_Thread.start(callback(this, &realtime_thread::loop));
    m_Ticker.attach(callback(this, &realtime_thread::sendSignal), microseconds{static_cast<int64_t>(m_Ts * 1e6f)});
}
