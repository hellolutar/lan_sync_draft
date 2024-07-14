#ifndef __PERSIST_FRAMEWORK_H_
#define __PERSIST_FRAMEWORK_H_

#include <memory>
#include <vector>

class PersistMonitor
{
private:
public:
    PersistMonitor(/* args */);

    /**
     * A callback that is reading data
     *
     * @note: It is the user's responsibility To free the data
     *
     * @param curpos: is the offset
     * @param data_len: is the readed data len
     * @param : data
     *
     */
    virtual void monitor(uint64_t from_pos, void *data, uint64_t data_len) = 0;
    virtual ~PersistMonitor() = 0;
};

class PersistFrameworkEgine
{
protected:
    std::vector<std::shared_ptr<PersistMonitor>> mons_;

public:
    PersistFrameworkEgine(/* args */) {}
    virtual ~PersistFrameworkEgine()
    {
        mons_.clear();
    }

    virtual void addReadMonitor(std::shared_ptr<PersistMonitor> monitor);

    /**
     * read data from the path.
     *
     * @param path      target file
     * @param ret_len   the size of the actual read
     * @return          file content
     *
     */
    virtual std::shared_ptr<uint8_t[]> readFrom(std::string path, uint64_t &ret_len) = 0;

    /**
     * keep content to the path
     *
     * @param path      target file
     * @param offset    the possition we want start to read in the target file
     * @param size      the size we want to read
     * @param ret_len   the size of the actual to read
     * @return          file content
     */
    virtual std::shared_ptr<uint8_t[]> readFrom(std::string path, uint64_t offset, uint64_t size, uint64_t &ret_len) = 0;

    /**
     * keep content to the path
     *
     * @param path      target file
     * @param offset    the possition we want start to write in the target file
     * @param data      the data we want to keep
     * @param size      the size we want to write
     * @return          the size of the actual to write
     *
     */
    virtual uint64_t saveTo(std::string path, uint64_t offset, uint8_t *data, uint64_t size) = 0;
};

class PersistFramework
{
protected:
    static std::shared_ptr<PersistFrameworkEgine> engine_;

public:
    PersistFramework(/* args */) {}
    virtual ~PersistFramework() {}

    static void init(std::shared_ptr<PersistFrameworkEgine> eg);
    static std::shared_ptr<PersistFrameworkEgine> getEngine();
};

#endif