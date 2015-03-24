//
//   Copyright (c) 2014  Bruno Bierbaumer, Andreas Heider
//
//
//   Code for writing EFI variables of the code taken from Finnbarr P. Murphy.
//
//
//   Switches between the integrated and dedicated GPU of a Macbook Pro 11,3
// 
 

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
 
#define BITS_PER_LONG (sizeof(unsigned long) * 8)
#define EFI_ERROR(x) ((x) | (1L << (BITS_PER_LONG - 1)))
 
#define EFI_SUCCESS     0
#define EFI_INVALID_PARAMETER   EFI_ERROR(2)
#define EFI_UNSUPPORTED         EFI_ERROR(3)
#define EFI_BAD_BUFFER_SIZE     EFI_ERROR(4)
#define EFI_BUFFER_TOO_SMALL    EFI_ERROR(5)
#define EFI_NOT_FOUND           EFI_ERROR(14)
#define EFI_OUT_OF_RESOURCES    EFI_ERROR(15)
 
#define EFI_VARIABLE_NON_VOLATILE       0x0000000000000001
#define EFI_VARIABLE_BOOTSERVICE_ACCESS 0x0000000000000002
#define EFI_VARIABLE_RUNTIME_ACCESS     0x0000000000000004
 
typedef struct {
    uint32_t  data1;
    uint16_t  data2;
    uint16_t  data3;
    uint8_t   data4[8];
} efi_guid_t;
 
typedef unsigned long efi_status_t;
 
typedef struct _efi_variable_t {
    const char    *VariableName;
    efi_guid_t    VendorGuid;
    uint32_t      DataSize;
    uint8_t       *Data;
    uint32_t      Attributes;
} __attribute__((packed)) efi_variable_t;
 
 
#define SYSFS_EFI_VARS "/sys/firmware/efi/efivars"
 
#define EFI_GPU_GUID \
    (efi_guid_t) {0xfa4ce28d,0xb62f,0x4c99,{0x9c,0xc3,0x68,0x15,0x68,0x6e,0x30,0xf9}}
 
 
int variable_to_name(efi_variable_t *var, char *name)
{
    char *p = name;
    efi_guid_t *guid = &(var->VendorGuid);
 
    if (!var->VariableName)
        return -1;
 
    strcpy (p, var->VariableName);
    p += strlen (p);
    p += sprintf (p, "-");
 
    sprintf(p, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        guid->data1, guid->data2, guid->data3,
        guid->data4[0], guid->data4[1], guid->data4[2], guid->data4[3],
        guid->data4[4], guid->data4[5], guid->data4[6], guid->data4[7]
    );
 
    return strlen (name);
}
 

efi_status_t write_variable (efi_variable_t *var) {
    int fd;
    size_t writesize;
    void *buffer;
    unsigned long total;
    char name[PATH_MAX];
    char filename[PATH_MAX];
 
    if (!var)
        return EFI_INVALID_PARAMETER;
 
    variable_to_name(var, name);
    snprintf(filename, PATH_MAX-1, "%s/%s", SYSFS_EFI_VARS, name);
    if (!filename )
        return EFI_INVALID_PARAMETER;
 
    buffer = malloc(var->DataSize + sizeof(uint32_t));
    if (buffer == NULL) {
        return EFI_OUT_OF_RESOURCES;
    }
 
    memcpy (buffer, &var->Attributes, sizeof(uint32_t));
    memcpy (buffer + sizeof(uint32_t), var->Data, var->DataSize);
    total = var->DataSize + sizeof(uint32_t);
 
    fd = open (filename, O_WRONLY | O_CREAT,
                         S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1) {
        free (buffer);
        return EFI_INVALID_PARAMETER;
    }
 
    writesize = write (fd, buffer, total);
    if (writesize != total) {
        free (buffer);
        close (fd);
        return EFI_INVALID_PARAMETER;
    }
 
    close (fd);
    free (buffer);
    return EFI_SUCCESS;
}
 
 
int usage(char *name)
{
    fprintf(stderr, "usage: %s [ -i | --integrated | -d | --dedicated ]\n", name);
    exit(1);
}
 
int switch_gpu(uint8_t integrated)
{
    int status = 0;
    efi_variable_t var;
    memset (&var, 0, sizeof(var));

    uint8_t value[4] = {integrated,0x00,0x00,0x00};

    var.VariableName = "gpu-power-prefs";
    var.VendorGuid = EFI_GPU_GUID;

    var.Data = value;
    var.DataSize = 4;
    var.Attributes = EFI_VARIABLE_NON_VOLATILE |
                     EFI_VARIABLE_BOOTSERVICE_ACCESS |
                    EFI_VARIABLE_RUNTIME_ACCESS;
    if (write_variable (&var) != EFI_SUCCESS) {
        fprintf (stderr, "Failed to write variable: %s\n", var.VariableName);
            status = 1;
    }

/*    var.VariableName = "gpu-active";
    var.VendorGuid = EFI_GPU_GUID;

    var.Data = value;
    var.DataSize = 4;
    var.Attributes = EFI_VARIABLE_NON_VOLATILE |
                     EFI_VARIABLE_BOOTSERVICE_ACCESS |
                     EFI_VARIABLE_RUNTIME_ACCESS;
    if (write_variable (&var) != EFI_SUCCESS) {
        fprintf (stderr, "Failed to write variable: %s\n", var.VariableName);
        status = 1;
    }*/
    return status;  
}

 
int main(int argc, char *argv[])
{
    int status = EFI_SUCCESS;
 
    if (argc == 2) {
        mount("efivarfs",SYSFS_EFI_VARS,"efivarfs",0,"");
        if (strcmp (argv[1], "-i") == 0 || strcmp (argv[1], "--integrated") == 0) {
            status = switch_gpu(1);
        }
        else if (strcmp (argv[1], "-d") == 0 || strcmp (argv[1], "--dedicated") == 0) {
            status = switch_gpu(0);
        }
        else {
            usage(argv[0]);
        }
    }
    else {
        usage(argv[0]);
    }

    exit(status);
}


