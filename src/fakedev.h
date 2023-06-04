/*
 * fakedev.h
 *
 *  Created on: Aug 11, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include "header.h"

typedef struct fakedev_pci_bus_s
{
	unsigned int domain;
	unsigned int bus;
} fakedev_pci_bus_s;

typedef struct fakedev_pci_s
{
	struct fakedev_pci_bus_s* bus;
	unsigned int number;
	unsigned int function;
} fakedev_pci_s;

HIDDEN const char* fakedev();

HIDDEN const char* fakedev_generate();

void HIDDEN fakedev_make_populated_bus(struct fakedev_pci_bus_s* bus);

void HIDDEN fakedev_make_device(struct fakedev_pci_s* pci);

int HIDDEN fakedev_bus_path(char* out, struct fakedev_pci_bus_s* bus);

int HIDDEN fakedev_device_path(char* out, struct fakedev_pci_s* pci);

int HIDDEN fakedev_generate_pci_bus(struct fakedev_pci_bus_s* bus);

int HIDDEN fakedev_generate_pci_device(struct fakedev_pci_s* pci);

void HIDDEN fakedev_make_usb(struct fakedev_pci_s* pci, unsigned int usb, unsigned int idVendor, unsigned int idProduct);

void HIDDEN fakedev_make_pci(struct fakedev_pci_s* pci, unsigned int vendor, unsigned int device, unsigned int class);
