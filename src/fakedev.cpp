/*
 * fakedev.cpp
 *
 *  Created on: September 20, 2023
 *      Author: rosne-gamingyt
 */

#include "fakedev.hpp"
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <random>
#include <sys/stat.h>

constexpr int FAKEDEV_ACCESS = (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

static char* _fakedev_path = nullptr;
static int _fakedev_init = 0;

const char* fakedev();

const char* fakedev_generate();

const char* fakedev()
{
    if (_fakedev_init)
        return _fakedev_path;

    fakedev_generate();
    return _fakedev_path;
}

const char* fakedev_generate()
{
    _fakedev_init = 1;
    _fakedev_path = new char[256];
    std::snprintf(_fakedev_path, 255, "/tmp/devXXXXXX");

    // Generate a random seed using std::random_device for improved randomness
    std::random_device rd;
    std::default_random_engine generator(rd());

    if (mkdtemp(_fakedev_path))
    {
        fakedev_pci_bus_s bus;
        bus.bus = 0;
        bus.domain = 0;
        fakedev_make_populated_bus(&bus);
    }
    return nullptr;
}

void fakedev_make_populated_bus(fakedev_pci_bus_s* bus)
{
    if (fakedev_generate_pci_bus(bus) == 0)
    {
        for (unsigned int i = 0; i < 6 + (std::rand() % 12); ++i)
        {
            fakedev_pci_s pci;
            pci.bus = bus;
            pci.function = std::rand() % 8;
            pci.number = std::rand() % 32;
            fakedev_make_device(&pci);
        }
    }
}

void fakedev_make_device(fakedev_pci_s* pci)
{
    if (fakedev_generate_pci_device(pci) == 0)
    {
        // 50% chance to make USB device
        if (std::rand() % 2)
            fakedev_make_usb(pci, std::rand() % 8, std::rand() % 0xFFFF, std::rand() % 0xFFFF);
        else
            fakedev_make_pci(pci, std::rand() % 0xFFFF, std::rand() % 0xFFFF, (std::rand() % 20) << 16);
    }
}

int fakedev_bus_path(char* out, fakedev_pci_bus_s* bus)
{
    return std::sprintf(out, "%s/pci%04x:%02x", _fakedev_path, bus->domain, bus->bus);
}

int fakedev_device_path(char* out, fakedev_pci_s* pci)
{
    return std::sprintf(out, "%s/pci%04x:%02x/%04x:%02x:%02x.%01x", _fakedev_path, pci->bus->domain, pci->bus->bus,
                        pci->bus->domain, pci->bus->bus, pci->number, pci->function);
}

int fakedev_generate_pci_bus(fakedev_pci_bus_s* bus)
{
    char temp[512];
    fakedev_bus_path(temp, bus);
    return mkdir(temp, FAKEDEV_ACCESS);
}

int fakedev_generate_pci_device(fakedev_pci_s* pci)
{
    char temp[512];
    fakedev_device_path(temp, pci);
    return mkdir(temp, FAKEDEV_ACCESS);
}

void fakedev_make_usb(fakedev_pci_s* pci, unsigned int usb, unsigned int idVendor, unsigned int idProduct)
{
    char filename[512];
    char path[512];
    FILE* fd;

    fakedev_device_path(path, pci);

    std::snprintf(filename, 511, "%s/usb%u", path, usb);
    if (mkdir(filename, FAKEDEV_ACCESS) == 0)
    {
        std::snprintf(filename, 511, "%s/usb%u/idVendor", path, usb);
        if ((fd = std::fopen(filename, "w")) != nullptr)
        {
            std::fprintf(fd, "%04x\n", idVendor);
            std::fclose(fd);
        }
        std::snprintf(filename, 511, "%s/usb%u/idProduct", path, usb);
        if ((fd = std::fopen(filename, "w")) != nullptr)
        {
            std::fprintf(fd, "%04x\n", idProduct);
            std::fclose(fd);
        }
    }
}

void fakedev_make_pci(fakedev_pci_s* pci, unsigned int vendor, unsigned int device, unsigned int deviceClass)
{
    char filename[512];
    char path[512];
    FILE* fd;

    fakedev_device_path(path, pci);

    std::snprintf(filename, 511, "%s/device", path);
    if ((fd = std::fopen(filename, "w")) != nullptr)
    {
        std::fprintf(fd, "0x%04x\n", device);
        std::fclose(fd);
    }
    std::snprintf(filename, 511, "%s/vendor", path);
    if ((fd = std::fopen(filename, "w")) != nullptr)
    {
        std::fprintf(fd, "0x%04x\n", vendor);
        std::fclose(fd);
    }
    std::snprintf(filename, 511, "%s/class", path);
    if ((fd = std::fopen(filename, "w")) != nullptr)
    {
        std::fprintf(fd, "0x%06x\n", deviceClass);
        std::fclose(fd);
    }
}