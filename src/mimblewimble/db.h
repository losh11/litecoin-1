#pragma once

#include <libmw/libmw.h>
#include <dbwrapper.h>

class MWDBBatch : public libmw::IDBBatch
{
public:
    MWDBBatch(CDBWrapper* pDB, const std::shared_ptr<CDBBatch>& pBatch)
        : m_pDB(pDB), m_pBatch(pBatch) {}

    void Write(const std::string& key, const std::vector<uint8_t>& value) final
    {
        m_pBatch->Write(key, value);
    }

    void Erase(const std::string& key) final
    {
        m_pBatch->Erase(key);
    }

    void Commit() final
    {
        m_pDB->WriteBatch(*m_pBatch);
    }

private:
    std::shared_ptr<CDBBatch> m_pBatch;
    CDBWrapper* m_pDB;
};

class MWDBIterator : public libmw::IDBIterator
{
public:
    MWDBIterator(CDBIterator* pIterator)
        : m_pIterator(std::unique_ptr<CDBIterator>(pIterator)) { }

    void Seek(const std::string& key) final
    {
        m_pIterator->Seek(key);
    }

    void Next() final
    {
        m_pIterator->Next();
    }

    bool GetKey(std::string& key) const final
    {
        return m_pIterator->GetKey(key);
    }

    bool Valid() const final
    {
        return m_pIterator->Valid();
    }

private:
    std::unique_ptr<CDBIterator> m_pIterator;
};

class MWDBWrapper : public libmw::IDBWrapper
{
public:
    MWDBWrapper(CDBWrapper* pDB) : m_pDB(pDB) { }

    bool Read(const std::string& key, std::vector<uint8_t>& value) const final
    {
        return m_pDB->Read(key, value);
    }

    std::unique_ptr<libmw::IDBIterator> NewIterator() final
    {
        return std::unique_ptr<libmw::IDBIterator>(new MWDBIterator(m_pDB->NewIterator()));
    }

    std::unique_ptr<libmw::IDBBatch> CreateBatch() final
    {
        return std::unique_ptr<libmw::IDBBatch>(new MWDBBatch(m_pDB, std::make_shared<CDBBatch>(*m_pDB)));
    }

private:
    CDBWrapper* m_pDB;
};