#include "Bulk.h"
#include <assert.h>
#include <cstring>

static constexpr auto kBeginDynamicBlock = "{";
static constexpr auto kEndDynamicBlock = "}";

Bulk::Bulk(std::size_t size)
    : _size{size},
    _started{0},
    _dynamicBlocksDepth{0},
    _nextCbIdx{0}
{
    _buffer.reserve(size);
}

int Bulk::signReadyRead(CallbackReadyRead cb) noexcept
{
    auto idx = _nextCbIdx;
    _nextCbIdx++;
    _cbs.insert(std::make_pair(idx, cb));

    return idx;
}

void Bulk::readNext(const std::string &line)
{
    static std::string_view beginDynamicBlock{kBeginDynamicBlock, strlen(kBeginDynamicBlock)};
    static std::string_view endDynamicBlock{kEndDynamicBlock, strlen(kEndDynamicBlock)};

    if (!line.empty()) {
        // если был начат динамический блок
        if (line == beginDynamicBlock) {
            if (!_buffer.empty() && _dynamicBlocksDepth == 0) {
                if (!_buffer.empty())
                    sendReadyRead(_started, _buffer);

                _buffer.clear();
            }

            _dynamicBlocksDepth++;
        // если был завершен или "завершен" динамический блок
        } else if (line == endDynamicBlock && _dynamicBlocksDepth > 0) {
            _dynamicBlocksDepth--;
            if (_dynamicBlocksDepth == 0) {
                if (!_buffer.empty())
                    sendReadyRead(_started, _buffer);

                clear();
            }
        } else {
            _buffer.push_back(line);

            // если был завершен обычный блок
            if (_dynamicBlocksDepth == 0 && _buffer.size() == _size) {
                if (!_buffer.empty())
                    sendReadyRead(_started, _buffer);

                clear();
            }
        }

        // простой способ запомнить время начала
        if (_buffer.size() == 1)
            _started = std::time(nullptr);
    } else {
        if (!_buffer.empty() && _dynamicBlocksDepth == 0)
            sendReadyRead(_started, _buffer);

        clear();
    }
}

void Bulk::sendReadyRead(time_t started, const std::vector<std::string> &values) const noexcept
{
    for (auto it = _cbs.begin(); it != _cbs.end(); ++it) {
        it->second(started, values);
    }
}

void Bulk::clear() noexcept
{
    _buffer.clear();
    _started = 0;
    _dynamicBlocksDepth = 0;
}
