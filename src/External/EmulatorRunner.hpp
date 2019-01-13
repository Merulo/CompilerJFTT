#include <string>

class EmulatorRunner
{
    public:
    bool emulate(std::string tmpFileName, std::string resultFileName);

    private:
    bool fileExists(const std::string& filename);
};