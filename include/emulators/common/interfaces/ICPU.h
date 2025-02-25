#pragma once

class ICPU {
    public:
        virtual ~ICPU() = default;

        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void reset() = 0;
};
