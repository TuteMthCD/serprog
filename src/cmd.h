#include <cstddef>
#include <cstdint>
#include <vector>

class Frame {
    public:
    Frame(uint8_t);
    Frame(Frame&&) = default;
    Frame(const Frame&) = default;
    Frame& operator=(Frame&&) = default;
    Frame& operator=(const Frame&) = default;
    ~Frame();

    typedef enum {
        ACK = 0x06, // Acknowledge (positive response)
        NAK = 0x15, // Negative Acknowledge (negative response)

        CMD_NOP = 0x00,            // Do nothing
        CMD_QUERY_IFACE = 0x01,    // Query interface version
        CMD_QUERY_COMMANDS = 0x02, // Query supported commands
        CMD_QUERY_NAME = 0x03,     // Query programmer name
        CMD_QUERY_SERBUF = 0x04,   // Query serial buffer size
        CMD_QUERY_BUSTYPE = 0x05,  // Query supported bus types
        CMD_QUERY_CHIPSIZE = 0x06, // Query chip size (2^n format)
        CMD_QUERY_OPBUF = 0x07,    // Query operation buffer size
        CMD_QUERY_WRN_MAX = 0x08,  // Query max length for multi-byte write

        CMD_READ_BYTE = 0x09,   // Read one byte
        CMD_READ_NBYTES = 0x0A, // Read N bytes

        CMD_OP_INIT = 0x0B,       // Initialize operation buffer
        CMD_OP_WRITE_BYTE = 0x0C, // Write byte with address into buffer
        CMD_OP_WRITE_N = 0x0D,    // Write N bytes into buffer
        CMD_OP_DELAY_US = 0x0E,   // Add delay in microseconds
        CMD_OP_EXECUTE = 0x0F,    // Execute operation buffer

        CMD_SYNC_NOP = 0x10,      // Special NOP that responds with NAK+ACK
        CMD_QUERY_RDN_MAX = 0x11, // Query max length for multi-byte read

        CMD_SET_BUSTYPE = 0x12,     // Set bus type(s) to use
        CMD_OP_SPI_TRANSFER = 0x13, // Perform SPI operation
        CMD_SET_SPI_FREQ = 0x14,    // Set SPI clock frequency
        CMD_SET_PIN_STATE = 0x15    // Enable/disable output drivers
    } opcode_t;

    size_t getOpcodeLen();
    size_t decodeInstr(uint8_t*, size_t);

    private:
    constexpr size_t opCodeLength(const opcode_t& cmd);

    opcode_t opcode;
    size_t opcode_len;

    std::vector<uint8_t> payload;

    uint32_t addr;
    uint32_t len;
};
