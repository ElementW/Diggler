# This spec is unfinished.

[TOC]

# Goals and mindset

The Diggler engine and this corresponding spec aims at creating a performant game "voxel" engine. Competing games include Terasology, Minecraft, Minetest, FreeMiner, and many others.

Most of these games offer some kind of modding ability, some provide default content. Diggler is aiming at only being an engine, whose content is uses is solely brought by mods. Emphasis is therefore made on having a fast engine and powerful modding abilities and features.

Diggler was born to clone the original Infiniminer game (the one that inspired Minecraft and all the others). It eventually became only an engine, and its lead developer, gravgun -- part of the MinetestForFun/Xorhub team --, made the following decision:

> Diggler must kill Minetest in every single point. Kill it so much noone will play Minetest again.
> Yes, this is an open war to the MT ecosystem and all of its core and game developers, with their awful, childish, stubborn and obnoxious mindset. They made an engine but make it stall. The whole codebase is a sum function of hacks on top of each other. Everything is badly implemented, unplanned, decided wrongly. Every single line has some problem you could comment about.

Design is one of the most important points of Diggler. Backwards compatibility is not, except for 1-version upgrades -- where Minetest wants to keep max backward compatibility and is why it **failed miserably**. Minecraft became popular even if it was awful in the beginning, because there were updates, to say, *incompatible updates*. Don't let history repeat itself.

If something can be done better, make it better. Break it. Break everything if you must. Let this be the engine's motto:

> Break it. Break it until it can't be made better.

Any "Don't break it if it works" mindset is **banned**. This is not how you get nice things -- not here at least.

## (planned) Features

* Cross-platform compatibility
  * Use OpenGL ES 2.0 to have maximum reach
     * Use the Angle project on Windows if the driver doesn't provide GLES2
* Few base gameplay mechanics. Things like health aren't even to be part of the base engine.
* Everything is a *registered* and *named* callback. No polling.
  * Registered to avoid traversing large callback trees. Traversing a limited callback index is faster.
  * Time deltas are sacred. Tick-based state computing must die.
* (Almost-)Arbitrary number of large Worlds in a Universe
  * Specific file headers (and corresponding footers) for easy recovery and verification in the case the Universe data gets lost/"deleted"
* Per-world and/or per-universe mods
* Mod and resource reloading
  * Resources: reload upon edit (using `inotify` on Linux)
* Floating chunksets: map chunks with non-integer position, movable and scalable
* Color voxel details: instancable detail (e.g. flowers) made out of colored cubes
* Dynamic content updates to the clients
* Partial/lazy content and script load/client upload
* Client side modding
* Cryptography-based authentication (can be coupled with SRP-exchanged password hash) and player/server identity verification
* Appendable data buffers to chunks, extending what can be represented for a given block (e.g. temperature)
  * Buffer processors that update these on a regular basis
     * Made with C++, calls back Lua (for efficiency)
* Full internationalization support
* On-disk and over-the-network compression support
  * Multiple algorithms depending on needs

## Games to take features from

* Minecraft
* Minetest (and FreeMiner)
* Terasology
* Cube World
* Infinifactory
* Infiniminer
* Galaxy55
* Trove

# Binary format details

## Numbers
Number types are denoted by the following table:
Symbol      | C(++) type
------------|--------------
`u8`/`byte` | `uint8_t`
`i8`        | `int8_t`
`u16`       | `uint16_t`
`i16`       | `int16_t`
`u32`       | `uint32_t`
`u32`       | `int32_t`
`u64`       | `uint64_t`
`u64`       | `int64_t`
`f`         | `float` (32-bit)
`d`         | `double` (64-bit)
All specified formats are little-endian, and floats/doubles are IEEE 754.

## Varints
(`u`)`varint`s are variable-length integers, stored in a big-endian fashion, each byte's most significant bit indicating continuation, and the actual binary data in the lower 7 bits.
`uvarint`s are, are their name implies, unsigned. Their signed counterpart (`varint`s) are encoded using a zigzag encoding, that is, `n` is encoded as `(n << 1) ^ (n >> B-1)` where `B` is the original integer's bit length.
Their max bit length are specified by a suffix, e.g. `varint64` -- note therefore (`u`)`varint8` would be pointless.
In case it isn't specified, the `varint` can virtually be of any length, indicating a bitfield (e.g. for flag storage) rather than an actual integer.
Their names are shortened to, respectively

* `v16`, `v32`, and `v64` for signed varints
* `uv16`, `uv32`, and `uv64` for unsigned varints
* `v` for bit fields

They come from [Google's Protobuf](https://developers.google.com/protocol-buffers/docs/encoding) library.

## Vectors
Vectors can be of dimensions 2 to 4. They are simply stored member-by-member, in `<x><y>[z][w]` order, with the type determined by the prefix.
E.g. `fvec3` is a 3-dimensional vector stored as 3 consecutive 32-bit IEEE754 floating-point numbers.

## Strings
Strings are arrays of arbitrary binary data -- usually text, in which case the UTF-8 encoding is used.
In cases where textual data is expected, UTF-8 Byte Order Markers are useless and must render all the rest of the string invalid, essentially considering it empty.
Implementations may replace invalid UTF-8-encoded characters by  replacement character "�" (`U+FFFD`).

* `string` = `uv32 length; byte data[length];`
* `sstring` (short string) = `u8 length; byte data[length];`
* `mstring` (medium string) = `u16 length; byte data[length];`
* `lstring` (long string) = `u32 length; byte data[length];`

## Arrays
Arrays are sequences of tightly-packed data of an unique type.

Their length as well as their type can be either hard-coded or determined from another variable.
They are denoted by:

*  *`type`*`[SIZE]`, `SIZE` being empty or a positive integer, e.g. `int[42]` or `float[]`
* *`type name`*`[SIZE]` when named

Where `type` is the type of the data elements.

When size isn't specified, their length is stored as an `uv32` before the array's tightly packed data in the byte stream.

## MessagePacks
A MessagePack **map** object, as defined by [the spec](https://github.com/msgpack/msgpack/blob/master/spec.md).
Written in this spec as `msgpack`. Optionally, the `msgpack<type>` notation may be used to indicate that all of the map's entries are of type `type`.

# General data structures, details and vocabulary

## Compression
CompID | Compression algorithm
------:|----------------------
     0 | None
     1 | [zlib](http://www.ietf.org/rfc/rfc1950.txt) ([DEFLATE](http://www.ietf.org/rfc/rfc1951.txt))
     2 | LZMA°
     3 | LZFX
     4 | LZO*
     5 | LZSS*
     6 | LZW*
     7 | [Snappy](http://google.github.io/snappy/)*
\* Listed for potential future use, currently unused
° Has a dictionary that can be relocated/shared

## Assets
Assets are just, conceptually, files. They can be transferred, compressed, be in a text or binary format, functional or non-functional, etc...

* AHash: SHA-256 hash of the asset's content.
  Text form: hexadecimal representation (a.k.a. [RFC4648](https://tools.ietf.org/html/rfc4648)§8).
* ABind: a triple consisting of an AHash, a mod name, and a (string) path, relative to the root of the aforementioned mod.
  Ideally a `std::tuple<AHash, std::string, std::string>` or the like, and represented as `<AHash>:<modname>:<path/file.ext>`.
* AGroup: array of ABinds.
  Text form: ABinds separated by newlines.
* AMeta: metadata about the asset, such as asset type and size. Additional info like author/contributors, license and related links may be added too. Only fetched once, additional info only being symbolic.
* ASourceList: array of sources from where a specific asset can be fetched.

### Formats
#### Image / texture

Format | 4CC
-------|----
[PNG](http://www.libpng.org/pub/png/) ([RFC 2083](https://tools.ietf.org/html/rfc2083)) | `PNG\0`


**Note**: [FLIF](http://flif.info/) may be added in the near future, probably once its format has stabilized.

#### Sound / music

Format | 4CC
-------|----
OGG [Vorbis](https://www.xiph.org/vorbis/) | `Vrbs`

**Note**: OGG [Opus](http://opus-codec.org/) (with `.opus` extension and `Opus` 4CC) may be added in the near future.

## Chunk status

* "emerged" means a chunk is fully ready to be read, modified, as well as written or read from/to persistent storage.
* "emerging" is the process of bringing a chunk to the emerged state.
* "generating" is a part of the emerging procedure, where chunk data is being created, implying said chunk wasn't already available from any available source. Unsaved chunks must go (once again) through this process if need be.

ID | Status
--:|:------
0  | Unemerged
1  | Needs re-emerging
2  | Emerged, unmodified
3  | Emerged, modified only by map updates
4  | Emerged, modified by non-player action (mobs, map update...)
5  | Emerged, modified by player action
6  | Emerged, empty

The existence of multiple "emerged" states is explained by the potential need to reduce the universe save file size, for example in case of a backup, or the whole universe being distributed online. This allows for safe save file trimming without loosing important parts of the map.

## Cryptography

Cryptography is implemented early for, as of now, 2 reasons:

 * No-interaction secure authentication. As convenient as passwords may be, being able to log in to a server with an identity you own without any additional user input is the best.
 * Future use cases, such as implementation of a "friend" system or the like

Signature keys are [Ed25519](https://en.wikipedia.org/wiki/EdDSA) keys.

* `PlrPSK` (Player Public Signature Key) and`PlrSSK` (Player Secret Signature Key)
* `SessPSK` (Session Public Signature Key) and `SessSSK` (Session Secret Signature Key)

## Players

* `PIID` (Player Instance ID): 64-bit relatively unique identifier for a player session.

### Password exchange

All password authentication is never to be done with plain-text passwords. Instead, the [SRP (RFC2945)](https://tools.ietf.org/html/rfc2945) procedure is used, using SHA-256 hashes in lieu of the MD5 hash algorithm the RFC mentions (as both MD5 and SHA1 are now to be considered weak and therefore not used anymore).

# Save files
## Overall structure
Universes are stored in folders containing the worlds it is made of as well as other (meta)data files.

### Header
Type      | Name  | Value
---------:|:------|-----------------
`byte[8]` | `HDs` | `\x01\xD1GLRusv`
`u32`     | `Hui` | Universe ID
`u8`      | `Hst` | Save file type
`u64`     | `HTs` | [UNIX Timestamp](https://en.wikipedia.org/wiki/Unix_time) of last edit
`u32`     | `Hfv` | Save format version
`v`       | `Hfl` | Flags
`u8`      | `HDc` | XOR checksum of previous bytes, `0xAA` as starting byte
`u8`      | `HDe` | `\x02`

### Footer

Type      | Name  | Value
---------:|:------|-----------------
`uint64`  | `Fcs` | 64-bit FastHash checksum of all previous bytes
`byte[8]` | `FTs` | `eu\xD1GLR\x04\x1C`

## Save file types
### Universe Root (`Hst = 0x00`)
```c++
string universeName;
string universeSubtitle;
msgpack<uint16> idMapping;
msgpack universeProperties;
uint32 worldIDs[];
```

### World Root (`Hst = 0x01`)
```c++
string worldName;
string worldSubtitle;
msgpack worldProperties;
uint32 areasIDs[];
```

### World Area (`Hst = 0x02`)
An Area equates to 8×8×8 = 512 Chunks, themselves storing 16×16×16 blocks each, resulting in a maximum of 2 097 152 blocks stored which in turn equates to 8 MB of (uncompressed) raw data if no extra buffers are added.

#### Chunk
```c++
struct Chunk {
  // This chunk's block info size
  uv32 size;
  if (size > 0) {
    uint8 compID; // Compression method
    v compFlags;
    compressed using @compID {
      uint16 ids[16*16*16]; // Block IDs
      uint16 data[16*16*16]; // Block data
      msgpack<?[16*16*16]> buffers; // See Bufferspecs section
      uint16 metadataCount;
      msgpack metadata[metadataCount];
    }
  } 
  // Entity data
  uv32 entityCount;
  struct Entity {
    uint64 id;
    fvec3 position; // Relative to the current Chunk's origin
    fvec3 velocity;
    datree properties;
  } entities[entityCount];
};
```
Block IDs and data are indexed in the flat array by `index = x + y*16 + z*16*16`.
 
Block data (let's call it `bdata`) matching the bitmask `0x8000` are metadata pointers -- that is, said block has a metadata MsgPack stored in `metadata` at index `bdata & 0x7FFFF`. This MsgPack can contain arbitrarily-defined data (as mods please) about the block. It has a special entry called `__data` containing the block's actual `bdata` if it wasn't a metadata pointer and isn't zero, and is the value returned by functions getting the block's data (directly writing to this field should **fail**).

#### Areas
```c++
uv32 areaCoordinates;
Chunk chunks[512];
```
Chunks are indexed in the flat array by `index = x + y*8 + z*8*8`.

# Bufferspecs
## Default bufferspecs
Bufferspec name | Type          | Description
----------------|---------------|------------
`light8`        | `u8`          | Light shade
`light555`      | `u16`         | Light color (RGB555)
`light8_8`      | `u8` + `u8`   | Light shade (2 params)
`light555_555`  | `u16` + `u16` | Light color (RGB555, 2 params)
`temperature`   | `f`           | Temperature, in degrees Celcius (°C)
`humidity`      | `f`
`electricity`   | `f` (U, voltage) + `f` (I, electric current)

## Mod-defined bufferspecs
Registered by mods upon their load. Like block names, they are prefixed with the mod identifier, e.g. `gases:entropy`.

## Bufferspec Processors
* `lightspread`
  * `falloff`
  * TBD (2nd light computation)
* `equilibrium`
  * Params TBD (conductivity, ...)

# Network
All network communication in Diggler is done via sequential messages, delivered by the [ENet library](http://enet.bespin.org) on multiple ENet channels.

## Message channels
Channels are merely used to provide traffic analysis and avoid blocking other messages in the message queue. They shouldn't be checked upon message arrival and processing.
Channel | Usage
--------|------
0       | Base
1       | Chat
2       | Life (player/entity spawn/death)
3       | Movement (player/entity movement)
4       | Metadata (Universe/World/entity metadata transfer)
5       | ChunkTransfer
6       | ChunkUpdate

## Message structure
```c++
enum MessageType : uint8 {
  Null = 0,

  ServerInfo = 220,

  PlayerJoin = 1,
  PlayerUpdate = 2,
  PlayerQuit = 3,
  ChunkTransfer = 4,
  ChunkUpdate = 5,
  StatsUpdate = 6,
  Event = 7,
  Chat = 8,
  Content = 9,

  ImplementationReserved = 240..255
}
struct Message {
  MessageType type;
  uint8 subtype;
  uvarint flags;
  byte data[];
}
```
`data` length is determined by the reported ENet packet's size, minus the header size.

## Message types
### ServerInfo
Can be sent while not yet connected. Used to retrieve info about a server, such as the player count, the gamemode being played, the server's icon/banner, etc...
```c++
subtype<ServerInfo>
enum ServerInfoSubtype : uint8 {
  Request = 0,
  Response = 1
}

messagedata<ServerInfo, Request>
struct ServerInfoRequest {
  sstring infos[];
}

messagedata<ServerInfo, Response>
struct ServerInfoResponse {
  msgpack infos;
}
```
The client asks for a list of information, and the server simply replies. Different infos are identified by a short string describing both what it is and optional parameters, separated by dot characters (`.`, U+002E).
The server may reply to the client with only some of the requested information, and/or information with different parameters, but is expected to do its best effort to send back the most appropriate information. It is up to the client to choose what to do with this data and put it to best use.

Parameters that expresses languages are substituted by `XXX` in the following table, and are expected to be ISO 639-3 alpha-3-*type* codes. Language codes that are not part of ISO 639-3 are considered **valid** and may very well be used for alternative languages, such as `zls` for 1337$P34|< ([leetspeak](https://en.wikipedia.org/wiki/Leet)).
When that language specification is shown between parentheses, it is optional and the server is free to return said information in any language if unspecified, whichever is the most appropriate (e.g. according to the server's homeland or client location guessed from its IP address).

Info name       | Reply type | Reply data
----------------|------------|-----------
`name`(`.XXX`)  | `string`   | Server name (in locale `XXX`)
`desc`(`.XXX`)  | `string`   | Description/subtitle (in locale `XXX`)
`web`(`.XXX`)   | `string`   | Server website (for locale `XXX`)
`icon`(`.X.Y`)  | `string`   | Server icon (pixel size `X`×`Y` pixels)
`banner`(`.X.Y`)| `string`   | Server banner (pixel size `X`×`Y` pixels)
`uptime`        | `uint32`   | Server uptime in seconds
`playerCount`   | `uint32`   | Number of online players
`playerNames`(`.N`) | `string[]` | Connected players' names (all by default, up to `N` if specified, in no specific order)

Additional information types may be created and used, but to avoid any conflicts with future official types they should be prefixed with `x-`.

#### Examples
Please note server behaviour is only *suggested*.
##### Example 1
Client with an IP bound to France in GeoIP database requests `name`, `desc` and `icon.32.32`.
Server has its `name` and `desc` in English and French, and only has its icon in 16×16 and 48×48 px form.
Given it is not specifically branded (nor configured) as being French or English, it'll send back `name.fra` and `desc.fra` because the client is French and did not specify a preference, and will also send `icon.48.48`, since sending the 16×16 px icon would be less than asked for but it has more.
##### Example 2 (server has less info than requested)
Client asks for `name.jpn` and `icon.48.48`.
Server only has its name in Turkish and its icon in 32×32 px format.
It will send back `name.tur` and `icon.32.32` since it's the only data it has, even if it's less than requested.

### PlayerJoin
```c++
subtype<PlayerJoin>
enum PlayerJoinSubtype : uint8 {
  Connect = 0,
  Confirm = 1
}

messagedata<PlayerJoin, Connect>
struct PlayerJoinConnect {
  sstring infos[];
}

messagedata<ServerInfo, Confirm>
struct ServerInfoResponse {
  msgpack infos;
}
```

### Content
```c++
subtype<Content>
enum ContentSubtype : uint8 {
  DefineBlocks,
  DeleteBlocks,
  DefineTexture,
  DeleteTexture
}

messagedata<Content, DefineBlocks>
struct ContentDefineBlocks {
  uint16 defCount;
  struct Definition {
    
    msgpack props;
  } defs[defCount];
}

messagedata<Content, DefineBlocks>
struct ContentDefineBlocks {
  uint16 defCount;
  msgpack defs[defCount];
}
```

# Unhandled cases

* Security concerns are part of this spec. However, not much concerns are taken in account when it comes to script secure sandboxing as well as potential exploits in the decoders of certain formats (such as PNG, `libpng` having [a growing history of CVE issues](http://libpng.org/pub/png/libpng.html)).
