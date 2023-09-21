/*
 * fakedev.hpp
 *
 *  Created on: September 20, 2023
 *      Author: rosne-gamingyt
 */

#pragma once

#include "header.hpp"

struct fakedev_pci_bus_s
{
	unsigned int domain;
	unsigned int bus;
};

struct fakedev_pci_s
{
	struct fakedev_pci_bus_s* bus;
	unsigned int number;
	unsigned int function;
};

extern const char* fakedev();

extern const char* fakedev_generate();

void fakedev_make_populated_bus(struct fakedev_pci_bus_s* bus);

void fakedev_make_device(struct fakedev_pci_s* pci);

int fakedev_bus_path(char* out, struct fakedev_pci_bus_s* bus);

int fakedev_device_path(char* out, struct fakedev_pci_s* pci);

int fakedev_generate_pci_bus(struct fakedev_pci_bus_s* bus);

int fakedev_generate_pci_device(struct fakedev_pci_s* pci);

void fakedev_make_usb(struct fakedev_pci_s* pci, unsigned int usbPort, unsigned int idVendor, unsigned int idProduct);

void fakedev_make_pci(struct fakedev_pci_s* pci, unsigned int vendor, unsigned int device, unsigned int deviceClass);