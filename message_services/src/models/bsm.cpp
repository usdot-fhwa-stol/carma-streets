#include <iostream>
#include <vector>
#include <string.h>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "bsm.h"

namespace message_services
{

    namespace models
    {

        bsm::bsm() : core_data(), header()
        {
        }

        bsm::~bsm()
        {
        }

        void bsm::fromJsonObject(const rapidjson::Value &obj)
        {
            if (obj.IsObject())
            {
                for (rapidjson::Value::ConstMemberIterator obj_itr = obj.MemberBegin(); obj_itr != obj.MemberEnd(); obj_itr++)
                {
                    if (obj_itr->value.IsObject() && boost::iequals(std::string(obj_itr->name.GetString()), std::string("header")))
                    {
                        header_t header;

                        for (rapidjson::Value::ConstMemberIterator header_itr = obj_itr->value.MemberBegin(); header_itr != obj_itr->value.MemberEnd(); header_itr++)
                        {
                            if (std::string(header_itr->name.GetString()) == std::string("year"))
                            {
                                header.year = header_itr->value.GetUint64();
                            }

                            if (std::string(header_itr->name.GetString()) == std::string("timestamp"))
                            {
                                header.timestamp = (header_itr->value.GetUint64());
                            }

                            if (std::string(header_itr->name.GetString()) == std::string("sec_mark"))
                            {
                                header.sec_mark = (header_itr->value.GetUint64());
                            }
                        }
                        setHeader(header);
                        spdlog::info("Header->timestamp : {0}", header.timestamp);
                    }

                    if (obj_itr->value.IsObject() && boost::iequals(std::string(obj_itr->name.GetString()), std::string("core_data")))
                    {
                        bsmCoreData_t core_data;
                        for (rapidjson::Value::ConstMemberIterator core_data_itr = obj_itr->value.MemberBegin(); core_data_itr != obj_itr->value.MemberEnd(); core_data_itr++)
                        {
                            if (std::string(core_data_itr->name.GetString()) == std::string("temprary_id"))
                            {
                                core_data.temprary_id = (core_data_itr->value.GetString());
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("sec_mark"))
                            {
                                core_data.sec_mark = (core_data_itr->value.GetInt64());
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("latitude"))
                            {
                                core_data.latitude = (core_data_itr->value.GetDouble());
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("longitude"))
                            {
                                core_data.longitude = (core_data_itr->value.GetDouble());
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("elev"))
                            {
                                core_data.elev = (core_data_itr->value.GetDouble());
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("heading"))
                            {
                                core_data.heading = (core_data_itr->value.GetDouble());
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("transmission"))
                            {
                                core_data.transmission = (core_data_itr->value.GetString());
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("speed"))
                            {
                                core_data.speed = (core_data_itr->value.GetDouble());
                            }

                            if (std::string(core_data_itr->name.GetString()) == std::string("size"))
                            {
                                for (rapidjson::Value::ConstMemberIterator size_itr = core_data_itr->value.MemberBegin(); size_itr != core_data_itr->value.MemberEnd(); size_itr++)
                                {
                                    if (boost::iequals(std::string(size_itr->name.GetString()), std::string("length")))
                                    {
                                        core_data.size.length = (size_itr->value.GetUint64());
                                    }
                                }
                            }

                            //Todo: Complete my other fields
                        }
                        setCore_data(core_data);
                    }
                }
            }
        }

        bool bsm::asJsonObject(rapidjson::Writer<rapidjson::StringBuffer> *writer) const
        {
            try
            {
                rapidjson::Value val_core_data;
                writer->StartObject();
                writer->Key("header");
                writer->StartObject();
                writer->Key("year");
                writer->String(std::to_string(this->getHeader().year).c_str());
                writer->Key("sec_mark");
                writer->String(std::to_string(this->getHeader().sec_mark).c_str());
                writer->Key("timestamp");
                writer->Uint64(this->getHeader().timestamp);
                writer->EndObject();
                writer->Key("bsmCoreData");
                writer->StartObject();
                writer->Key("temprary_id");
                writer->String(this->getCore_data().temprary_id.c_str());
                writer->String("secMark");
                writer->String(std::to_string(this->getCore_data().sec_mark).c_str());
                writer->String("latitude");
                writer->String(std::to_string(this->getCore_data().latitude).c_str());
                writer->String("longitude");
                writer->String(std::to_string(this->getCore_data().longitude).c_str());
                writer->String("elev");
                writer->String(std::to_string(this->getCore_data().elev).c_str());
                writer->String("heading");
                writer->String(std::to_string(this->getCore_data().heading).c_str());
                writer->String("transmission");
                writer->String(this->getCore_data().transmission.c_str());
                writer->String("speed");
                writer->String(std::to_string(this->getCore_data().speed).c_str());
                writer->EndObject();
                writer->EndObject();
                return true;
            }
            catch (std::exception &ex)
            {
                spdlog::critical("BSM model: asJsonObject error.: ", ex.what());
                return false;
            }
        }

        std::ostream &operator<<(std::ostream &out, bsm &bsm_obj)
        {
            std::cout << std::fixed;
            std::cout << std::setprecision(6);
            std::cout << "bsm model: year =  " << bsm_obj.header.year
                      << ", secMark =" << bsm_obj.header.sec_mark
                      << ", timestamp = " << bsm_obj.header.timestamp
                      << ", core_data: { temprary_id = " << bsm_obj.core_data.temprary_id << ", sec_mark = " << bsm_obj.core_data.sec_mark
                      << ", latitude= " << bsm_obj.core_data.latitude << ", longitude = " << bsm_obj.core_data.longitude
                      << ", size: {length = " << bsm_obj.core_data.size.length << "}, speed = " << bsm_obj.core_data.speed << "}" << std::endl;
        }

        header_t bsm::getHeader() const
        {
            return this->header;
        }

        void bsm::setHeader(header_t header)
        {
            this->header = header;
        }

        bsmCoreData_t bsm::getCore_data() const
        {
            return this->core_data;
        }

        void bsm::setCore_data(bsmCoreData_t core_data)
        {
            this->core_data = core_data;
        }
    }
}