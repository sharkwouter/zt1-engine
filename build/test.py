#!/usr/bin/env python3

from dataclasses import dataclass


dos_header_size = 64
nt_header_offset_value_location = 60

with open("lang0.dll", "rb") as fd:
  data = fd.read()
dos_header_data = data[:dos_header_size]
print(f"Dos header: {dos_header_data}")
nt_header_offset = int.from_bytes(data[nt_header_offset_value_location:dos_header_size], byteorder='little')

nt_header_data = data[nt_header_offset : nt_header_offset + 8]
print(f"NT header: {nt_header_data}")

file_header_offset = nt_header_offset + 4
file_header_size = 32
file_header_data = data[file_header_offset: file_header_offset + file_header_size]
print(f"File header: {file_header_data}")
machine = int.from_bytes(data[file_header_offset: file_header_offset + 2], byteorder='little')
match(machine):
  case 0x8864:
    print("Machine type is AMD64")
  case 0x14c:
    print("Machine type is i386")
number_of_sections = int.from_bytes(data[file_header_offset + 2: file_header_offset + 2 + 2], byteorder='little')
print(f"Number of sections is {number_of_sections}")
size_of_optional_header = int.from_bytes(data[file_header_offset + 16: file_header_offset + 16 + 2], byteorder='little')
print(f"Size of optional header {size_of_optional_header}")

# It turns out a 32 bit optional header has 31 members, 64 bit has 30. BaseOfData is exclusive to 32 bit
# In 32 bit some values are ulonglong instead of dword, which are ImageBase, SizeOfStackReserve, SizeOfStackCommit, SizeOfHeapReserve and SizeOfHeapCommit
optional_header_offset = file_header_offset + 20
optional_header_magic = int.from_bytes(data[optional_header_offset : optional_header_offset + 2], byteorder='little')
match(optional_header_magic):
  case 0x10B:
    print("Magic says PE32")
  case 0x20B:
    print("Magic says PE32+")
  case 0x107:
    print("Magic says ROM image")
size_of_all_headers = int.from_bytes(data[optional_header_offset + 60 : optional_header_offset + 60 + 4], byteorder='little')
print(f"Size of all headers is {size_of_all_headers}")
file_alignment = int.from_bytes(data[optional_header_offset + 32 : optional_header_offset + 32 + 2], byteorder='little')
section_alignment = int.from_bytes(data[optional_header_offset + 30 : optional_header_offset + 30 + 2], byteorder='little')
print(f"File alignment is {file_alignment}")
print(f"Section alignment is {section_alignment}")
number_of_data_directories = int.from_bytes(data[optional_header_offset + 92 : optional_header_offset + 92 + 4], byteorder='little')
print(f"Number of data directories {number_of_data_directories}")

# This is part of the optional headers, but it is all the way at the end
data_directories_offset = optional_header_offset + 96

print(f"Data directories list offset is {data_directories_offset}")

@dataclass
class data_directory:
  virtual_address : int
  size : int

# Data directories are more for the kernel it seems, I might not need them
data_directory_infos = []
for memory_location in range(data_directories_offset, data_directories_offset + 8 * (number_of_data_directories), 8):
  data_directory_infos.append(
    data_directory(
      virtual_address = int.from_bytes(data[memory_location : memory_location + 4], byteorder='little', signed=False),
      size = int.from_bytes(data[memory_location +4 : memory_location + 4 + 4], byteorder='little'),
    )
  )
for index, info in enumerate(data_directory_infos):
  print(f"Found data directory {index} at {info.virtual_address} of size {info.size}")
  if (index == 2):
    print("This is the IMAGE_DIRECTORY_ENTRY_RESOURCE")

section_header_offset = data_directories_offset + 8 * (number_of_data_directories)

@dataclass
class section_header:
  name : str  # This one can only be up to 8 characters/bytes
  virtualSize : int
  virtualAddress: int
  SizeOfRawData : int
  PointerToRawData: int
  PointerToRelocations: int
  NumberOfRelocations: int

section_headers = []
section_header_size = 40
for memory_location in range(section_header_offset, section_header_offset + (number_of_sections * section_header_size), section_header_size):
  section_headers.append(
    section_header(
      name = data[memory_location : memory_location + 8].decode("ASCII"),  # Could be ASCII too, dunno
      virtualSize = int.from_bytes(data[memory_location + 8 : memory_location + 8 + 4], byteorder='little'),
      virtualAddress= int.from_bytes(data[memory_location + 12 : memory_location + 12 + 4], byteorder='little'),
      SizeOfRawData= int.from_bytes(data[memory_location + 16 : memory_location + 16 + 4], byteorder='little'),
      PointerToRawData= int.from_bytes(data[memory_location + 20 : memory_location + 20 + 4], byteorder='little'),
      PointerToRelocations= int.from_bytes(data[memory_location + 24 : memory_location + 24 + 4], byteorder='little'),
      NumberOfRelocations= int.from_bytes(data[memory_location + 28 : memory_location + 28 + 2], byteorder='little'),
    )
  )

for header in section_headers:
  print(header)
  if (header.name.startswith(".rsrc")):
    break;

resource_data = data[header.virtualAddress : header.virtualAddress + header.virtualSize]

__IMAGE_RESOURCE_DIRECTORY_format__ = (
  "IMAGE_RESOURCE_DIRECTORY",
  (
    "I,Characteristics",
    "I,TimeDateStamp",
    "H,MajorVersion",
    "H,MinorVersion",
    "H,NumberOfNamedEntries",
    "H,NumberOfIdEntries",
  ),
)

_IMAGE_RESOURCE_DIRECTORY_ENTRY_format__ = (
  "IMAGE_RESOURCE_DIRECTORY_ENTRY",
  ("I,Name", "I,OffsetToData"),
)

for index, info in enumerate(data_directory_infos):
  if (index == 2):
    NumberOfNamedEntries = int.from_bytes(data[info.virtual_address + 12 : info.virtual_address + 12 + 2], byteorder='little')
    NumberOfIdEntries = int.from_bytes(data[info.virtual_address + 14 : info.virtual_address + 14 + 2], byteorder='little')
    print(f"Number of named entries in resource directory: {NumberOfNamedEntries}")
    print(f"Number of id entries in resource directory: {NumberOfIdEntries}")
    
    @dataclass
    class resource_entry:
      name: int
      offsetToData : int
    entries = []
    resource_entries_offset = info.virtual_address +16
    for entry in range(NumberOfIdEntries + NumberOfNamedEntries):
      current_entry_offset = resource_entries_offset + 8 * entry 
      entries.append(
        resource_entry(
          name = int.from_bytes(data[current_entry_offset : current_entry_offset + 4], byteorder='little', signed=False),
          offsetToData = int.from_bytes(data[current_entry_offset + 4 : current_entry_offset + 4 + 4], byteorder='little'),
        )
      )
    for entry in entries:
      try:
        print(data[entry.offsetToData : entry.offsetToData + 12].decode("ASCII"))
      except:
        pass
      is_string = (entry.name & 0x80000000) >> 31
      if (is_string):
        NameOffset = entry.name & 0x7FFFFFFF
        # print(data[resource_data + NameOffset : resource_data+ NameOffset + 120].decode("ASCII"))
      else:
        resource_Id = entry.name & 0x0000FFFF
        # print(resource_Id)
        if (entry.name == 7110):
          exit(0)
          pass