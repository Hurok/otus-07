#ifndef BULK_H
#define BULK_H

#include <vector>
#include <functional>
#include <string>
#include <optional>
#include <ctime>
#include <map>

/*!
 * \brief Класс для обработки массива данных. Подготовленные данные отдаются в CallbackReadyRead.
 */
class Bulk
{
public:
    typedef std::function<void(std::time_t started, const std::vector<std::string> &)> CallbackReadyRead;

    Bulk(std::size_t size);
    ~Bulk() = default;

    /*!
     * \brief Обработанные данные.
     */
    int signReadyRead(CallbackReadyRead cb) noexcept;
    void unsign(int idx) noexcept;

    /*!
     * \brief Разбор следующей строки данных. В случае ошибки генерирует исключение std::logic_error.
     * \return Возвращает false, если данные закончились.
     */
    void readNext(const std::string &line);

private:
    /*!
     * \brief Отправляет сигнал CallbackReadyRead всем подсписчикам.
     */
    void sendReadyRead(std::time_t started, const std::vector<std::string> &values) const noexcept;

    /*!
     * \brief Очищает все текущие данные.
     */
    void clear() noexcept;

    const std::size_t _size;
    std::vector<std::string> _buffer;
    std::time_t _started;
    std::map<int, CallbackReadyRead> _cbs;
    int _dynamicBlocksDepth;
    int _nextCbIdx;
};

#endif // BULK_H
