SECTOR_SIZE = 512

def read_sector(img_path, sector_num):
    with open(img_path, "rb") as f:
        f.seek(sector_num * SECTOR_SIZE)
        return f.read(SECTOR_SIZE)

def list_directory(sector):
    num_entries = sector[0]
    entries = []
    for i in range(num_entries):
        entry = sector[1 + i*32 : 1 + (i+1)*32]
        name = entry[0:15].rstrip(b'\x00').decode()
        entry_type = entry[16]
        content_sector = entry[17]
        entries.append((name, entry_type, content_sector))
    return entries

root = read_sector("/home/zyz/Documentos/Projetos/knightos/disk.img", 1)
for name, typ, sector in list_directory(root):
    print(f"{name} - {'DIR' if typ==1 else 'FILE'} - sector {sector}")
