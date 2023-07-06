#pragma once

#include "..\Utils\JsmnStreamXX.hpp"
#include "..\Utils\SatAlloc.hpp"

template <typename... SupportedTypes>
class EntityLoader
{
private:
    struct Parser : public JsmnStreamXX<32, 64>
    {
        bool startParsing;
        size_t currentEntityId = EntityCapacity;
        char componentName[32];

        void Init()
        {
            startParsing = false;
            currentEntityId = EntityCapacity;
            componentName[0] = '\0';
        }

        Parser()
        {
            Init();
        }

        void Process(Action action, const char *string, size_t stringLength)
        {
            switch (action)
            {
            case Action::ObjectStart:
                if (startParsing)
                    currentEntityId = Entity<SupportedTypes...>::Create().GetId();
                break;
            case Action::ObjectKey:
                if (startParsing)
                    strcpy(componentName, string);
                else if (!strcmp(string, "Entities"))
                    startParsing = true;
                break;
            case Action::String:
                if (startParsing)
                    Entity<SupportedTypes...>::GetEntity(currentEntityId).AddComponent((const char *)componentName, string);
                break;
            default:
                break;
            }
        }
    };

    static Parser *parser = new Parser();

#define DRDY (0x0002) /* Data transfer preparations complete */
#define EHST (0x0080) /* Host I/O processing complete */
#define CD_STATUS_TIMEOUT 0xAA
#define DTR 0x0000UL
#define HIRQ 0x0008UL

    static iso9660_filelist_entry_t FindFile(const char *fileName)
    {
        iso9660_filelist_entry_t file = {};
        iso9660_filelist_t *fileList = (iso9660_filelist_t *)lwram::malloc(sizeof(iso9660_filelist_t));
        fileList->entries = (iso9660_filelist_entry_t *)lwram::malloc(sizeof(iso9660_filelist_entry_t) * ISO9660_FILELIST_ENTRIES_COUNT);

        fileList->entries_count = 0;
        fileList->entries_pooled_count = 0;

        iso9660_filelist_root_read(fileList, -1);
        for (size_t i = 0; i < fileList->entries_count; i++)
            if (!strcmp(fileList->entries[i].name, fileName))
                file = fileList->entries[i];

        lwram::free(fileList->entries);
        lwram::free(fileList);
        return file;
    }

    template <typename T, typename A>
    static volatile T &RefFrom(A address)
    {
        return *((volatile T *)address);
    }

    static int flag_wait(uint16_t flag)
    {
        volatile uint32_t timeout = 0x240000;

        while (timeout--)
            if (RefFrom<uint16_t>(CD_BLOCK(HIRQ)) & flag)
                return 0;

        return -1;
    }

    static int ProcessData(uint16_t offset, uint16_t buffer_number, uint32_t bytes_to_read, uint32_t sectors_to_read)
    {

        /* Start transfer */
        int ret;
        ret = cd_block_cmd_sector_data_get_delete(offset, buffer_number, sectors_to_read);
        if (ret != 0)
        {
            return ret;
        }

        /* Wait for data */
        if ((flag_wait(DRDY | EHST)) != 0)
        {
            return CD_STATUS_TIMEOUT;
        }

        char valueRead[2] = {};
        uint32_t read_bytes = 0;
        for (uint32_t i = 0; i < bytes_to_read; i += 2)
        {
            *((uint16_t *)valueRead) = MEMORY_READ(16, CD_BLOCK(DTR));
            parser->Parse(valueRead[0]);
            parser->Parse(valueRead[1]);
            read_bytes += 2;
        }

        /* If odd number of bytes, read the last one separated */
        if (read_bytes < bytes_to_read)
        {
            *((uint16_t *)valueRead) = MEMORY_READ(16, CD_BLOCK(DTR));
            parser->Parse(valueRead[0]);
        }

        if ((ret = cd_block_cmd_data_transfer_end()) != 0)
        {
            return ret;
        }

        return 0;
    }

public:
    static int Load()
    {
        iso9660_filelist_entry_t file = FindFile("ENTITIES");

        fad_t fad = file.starting_fad;
        uint32_t length = file.size;

        assert(fad >= 150);
        assert(length > 0);

        /* Get the sector count from length */
        const uint32_t sector_count = (length + (ISO9660_SECTOR_SIZE - 1)) / ISO9660_SECTOR_SIZE;

        int ret;

        if ((ret = cd_block_cmd_selector_reset(0, 0)) != 0)
        {
            return ret;
        }

        if ((ret = cd_block_cmd_cd_dev_connection_set(0)) != 0)
        {
            return ret;
        }

        if ((ret = cd_block_cmd_disk_play(0, fad, sector_count)) != 0)
        {
            return ret;
        }

        uint32_t bytes_missing = length;
        while (bytes_missing > 0)
        {
            /* Wait until there's data ready */
            uint32_t sectors_ready;

            do
            {
                sectors_ready = cd_block_cmd_sector_number_get(0);
            } while (sectors_ready == 0);

            uint32_t bytes_to_read;

            if ((sectors_ready * ISO9660_SECTOR_SIZE) > bytes_missing)
            {
                bytes_to_read = bytes_missing;
            }
            else
            {
                bytes_to_read = sectors_ready * ISO9660_SECTOR_SIZE;
            }

            if ((ret = ProcessData(0, 0, bytes_to_read, sectors_ready)) != 0)
            {
                return ret;
            }

            bytes_missing -= bytes_to_read;
        }
        return 0;
    }
};
