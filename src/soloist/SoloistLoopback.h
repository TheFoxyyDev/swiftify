#pragma once

#include <QtGlobal>

#include <atomic>
#include <functional>
#include <thread>

namespace swiftify {

/// what soloist plays, heard on windows, where it can't be told to play into
/// a sink of swiftify's own: a process loopback of soloist (and anything it
/// starts), recorded for the visualizers alone while soloist goes on playing
/// to the speakers. needs windows 10 2004 or later; on older ones the
/// recording fails, is logged, and the visualizers stay still as before.
class SoloistLoopback {
public:
    /// the rate the analyser's timing assumes (librespot's); windows
    /// converts soloist's audio to it
    static constexpr quint32 kRate = 44100;
    static constexpr quint32 kChannels = 2;
    /// interleaved stereo at kRate, on the recording thread. a second of
    /// silence follows when soloist stops playing, so the bars fall; then
    /// nothing until it plays again
    using Sink = std::function<void(const float *samples, qsizetype count)>;

    explicit SoloistLoopback(Sink sink);
    ~SoloistLoopback();
    SoloistLoopback(const SoloistLoopback &) = delete;
    SoloistLoopback &operator=(const SoloistLoopback &) = delete;

    /// records `processId`'s audio from now on, in place of any recorded before
    void start(quint32 processId);
    void stop();

private:
    void record(quint32 processId);

    Sink m_sink;
    std::thread m_thread;
    std::atomic<bool> m_stopping = false;
    /// set to wake the recording thread when it is to stop
    void *m_wake = nullptr;
};

} // namespace swiftify
