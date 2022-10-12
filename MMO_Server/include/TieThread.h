#pragma once
#include "FrameTime.h"

namespace tie
{
class thread
{
private:
    //Make container for mutex, condition_variable
    class pMtx
    {
    public:
        std::shared_ptr<std::mutex> m;
        pMtx( std::shared_ptr<std::mutex> mtx): m(mtx){};
    };
    class pCV
    {
    public:
        std::shared_ptr<std::condition_variable> cv;
        pCV( std::shared_ptr<std::condition_variable> _cv): cv(_cv){};
    };
public:
    //Sequence thread in container
    enum class SeqID_Thread
    {
        tTimer_FPS,
        Amount_Thread
    };

    thread()
    {
        for( int i = 0; i < (int)SeqID_Thread::Amount_Thread; i++ )
        {
            std::shared_ptr<std::mutex> mtx( new std::mutex );
            pMtx newMtx(mtx);
            vMutexRosters.emplace_back( pMtx(newMtx) );

            std::shared_ptr<std::condition_variable> cv(new std::condition_variable);
            pCV newCV(cv);
            vCVRosters.emplace_back( pCV(newCV) );
        }
    }
private:
    //Container include mutex, condition
    std::vector<pMtx> vMutexRosters;
    std::vector<pCV> vCVRosters;
    std::mutex mtx_main;
    static tie::thread& Get()
    {
        static tie::thread tie_thr;
        return tie_thr;
    }
    std::shared_ptr<std::mutex>& _GetMutex( SeqID_Thread seqid )
    {
        return vMutexRosters[(int)seqid].m;
    }
    std::shared_ptr<std::condition_variable>& _GetConditionVar( SeqID_Thread seqid )
    {
        return vCVRosters[(int)seqid].cv;
    }
public:    
    static std::shared_ptr<std::mutex>& GetMutex( SeqID_Thread seqid )
    {
        return Get()._GetMutex(seqid);
    }

    static std::shared_ptr<std::condition_variable>& GetConditionVar( SeqID_Thread seqid )
    {
        return Get()._GetConditionVar( seqid );
    }
public:
    template <typename _Func, typename... _Args>
    static void AddThread(_Func &&func, _Args &&...args)
    {
        std::thread newThread(std::forward<_Func>(func), std::forward<_Args>(args)...);
        if (newThread.joinable()) newThread.detach();
    }

    template<typename... _Args>
    static void MakeLoop( int fps, void (*func)(_Args...), _Args&&... args )
    {
        static FrameTime ft_ml;
        auto next_frame = std::chrono::steady_clock::now();
        while (true)
        {
            next_frame += std::chrono::microseconds(1000000 / fps);
            
            func( std::forward<_Args>(args)... );

            std::this_thread::sleep_until(next_frame);
        }
    }

    static void LoopWakeUpThread()
    {
        std::lock_guard<std::mutex> lock(Get().mtx_main);
        //Loop wakeup thread
        FrameTime ft_loopwakeup;
        auto next_frame = std::chrono::steady_clock::now();
        while (true)
        {
            auto mrs = 100;//100us per frame
            next_frame += std::chrono::microseconds(mrs);
            // float dTime = ft_loopwakeup.Mark();
            // std::cout << "Dura Loop: " << dTime*1000 << "ms" << std::endl;
            //WakeUp condition variable of each thread in container
            for( auto& t : Get().vCVRosters ) t.cv->notify_one();
            
            std::this_thread::sleep_until(next_frame);
        }
    }
    void CountDown(int& dura);
};
}