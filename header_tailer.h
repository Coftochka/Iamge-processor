#include <cstdlib>
#include <utility>
#include <fstream>
class Header {
public:
    Header();
    explicit Header(std::ifstream& stream);
    ~Header();
    void SetSize(std::pair<size_t, size_t> size);
    u_int32_t GetWidth() const;
    u_int32_t GetHeight() const;
    u_int32_t ArraySize() const;
    const char* BufferBegin() const;
    u_int32_t HeaderSize() const;
    Header& operator=(const Header& h2);
    void Clear();

private:
    char* buffer_;
    size_t filesize_;
    size_t pixel_begin_;
    u_int32_t width_;
    u_int32_t height_;
    // потому что в bmp они хранятся в 4 байта. не больше, не меньше.
};