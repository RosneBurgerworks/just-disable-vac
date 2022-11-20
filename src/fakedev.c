/*
 * fakedev.c
 *
 *  Created on: Aug 11, 2017
 *      Author: nullifiedcat
 */

#include "fakedev.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

HIDDEN char* _fakedev_path = 0;
HIDDEN int _fakedev_init = 0;

#define FAKEDEV_ACCESS (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)

HIDDEN const char* fakedev()
{
	if (_fakedev_init)
		return (const char*)_fakedev_path;

	fakedev_generate();
	return (const char*)_fakedev_path;
}

HIDDEN const char* fakedev_generate()
{
	_fakedev_init = 1;
	_fakedev_path = malloc(256);
	snprintf(_fakedev_path, 255, "/tmp/devXXXXXX");
	if (mkdtemp(_fakedev_path))
	{
		struct fakedev_pci_bus_s bus;
		bus.bus = 0;
		bus.domain = 0;
		fakedev_make_populated_bus(&bus);
	}
	return 0;
}

void HIDDEN fakedev_make_populated_bus(struct fakedev_pci_bus_s* bus)
{
	if (0 == fakedev_generate_pci_bus(bus))
	{
		for (int i = 0; i < (6 + rand() % 12); i++)
		{
			struct fakedev_pci_s pci;
			pci.bus = bus;
			pci.function = rand() % 8;
			pci.number = rand() % 32;
			fakedev_make_device(&pci);
		}
	}
}

void HIDDEN fakedev_make_device(struct fakedev_pci_s* pci)
{
	if (0 == fakedev_generate_pci_device(pci))
	{
		// 50% chance to make USB device
		if (rand() % 2)
			fakedev_make_usb(pci, rand() % 8, rand() % 0xFFFF, rand() % 0xFFFF);
		else
			fakedev_make_pci(pci, rand() % 0xFFFF, rand() % 0xFFFF, (rand() % 0x14) << 16);

	}
}

int HIDDEN fakedev_bus_path(char* out, struct fakedev_pci_bus_s* bus)
{
	return sprintf(out, "%s/pci%04x:%02x", _fakedev_path, bus->domain, bus->bus);
}

int HIDDEN fakedev_device_path(char* out, struct fakedev_pci_s* pci)
{
	return sprintf(out, "%s/pci%04x:%02x/%04x:%02x:%02x.%01x", _fakedev_path, pci->bus->domain, pci->bus->bus,
		pci->bus->domain, pci->bus->bus, pci->number, pci->function);
}

int HIDDEN fakedev_generate_pci_bus(struct fakedev_pci_bus_s* bus)
{
	char temp[512];
	fakedev_bus_path(temp, bus);
	return mkdir(temp, FAKEDEV_ACCESS);
}

int HIDDEN fakedev_generate_pci_device(struct fakedev_pci_s* pci)
{
	char temp[512];
	fakedev_device_path(temp, pci);
	return mkdir(temp, FAKEDEV_ACCESS);
}

void HIDDEN fakedev_make_usb(struct fakedev_pci_s* pci, unsigned usb, unsigned idVendor, unsigned idProduct)
{
	char filename[512];
	char path[512];
	FILE* fd = 0;

	fakedev_device_path(path, pci);

	snprintf(filename, 511, "%s/usb%u", path, usb);
	if (0 == mkdir(filename, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))
	{
		snprintf(filename, 511, "%s/usb%u/idVendor", path, usb);
		if (0 != (fd = fopen(filename, "w")))
		{
			fprintf(fd, "%04x\n", idVendor);
			fclose(fd);
		}
		snprintf(filename, 511, "%s/usb%u/idProduct", path, usb);
		if (0 != (fd = fopen(filename, "w")))
		{
			fprintf(fd, "%04x\n", idProduct);
			fclose(fd);
		}
	}
}

void HIDDEN fakedev_make_pci(struct fakedev_pci_s* pci, unsigned vendor, unsigned device, unsigned class)
{
	char filename[512];
	char path[512];
	FILE* fd = 0;

	fakedev_device_path(path, pci);

	snprintf(filename, 511, "%s/device", path);
	if (0 != (fd = fopen(filename, "w")))
	{
		fprintf(fd, "0x%04x\n", device);
		fclose(fd);
	}
	snprintf(filename, 511, "%s/vendor", path);
	if (0 != (fd = fopen(filename, "w")))
	{
		fprintf(fd, "0x%04x\n", vendor);
		fclose(fd);
	}
	snprintf(filename, 511, "%s/class", path);
	if (0 != (fd = fopen(filename, "w")))
	{
		fprintf(fd, "0x%06x\n", class);
		fclose(fd);
	}
}
