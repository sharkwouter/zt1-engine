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
  
  data_directory_infos = []
  for memory_location in range(data_directories_offset, data_directories_offset + 8 * (number_of_data_directories), 8):
    data_directory_infos.append(
      data_directory(
        virtual_address = int.from_bytes(data[memory_location : memory_location + 4], byteorder='little'),
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
    physical_address : int
    SizeOfRawData : int

  section_headers = []
  section_header_size = 40
  for memory_location in range(section_header_offset, section_header_offset + (number_of_sections * section_header_size), section_header_size):
    section_headers.append(
      section_header(
        name = data[memory_location : memory_location + 8].decode("ASCII"),  # Could be ASCII too, dunno
        physical_address = int.from_bytes(data[memory_location + 8 : memory_location + 8 + 4], byteorder='little'),
        SizeOfRawData= int.from_bytes(data[memory_location + 12 : memory_location + 12 + 4], byteorder='little'),
      )
    )
  
  for header in section_headers:
    print(header)

