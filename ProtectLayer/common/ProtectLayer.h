#ifndef PROTECTLAYER_H
#define PROTECTLAYER_H

#include "ProtectLayerGlobals.h"
#include "Crypto.h"
#include "KeyDistrib.h"
#include "CTP.h"

#define ENABLE_UTESLA   1   // TODO move to Makefile

// #undef __linux__ // TODO!!! REMOVE - just for VS Code syntax highliting

#ifdef __linux__

#include "uTESLAMaster.h"
#include "configurator.h"

#include <string>

#else 

#include "uTESLAClient.h"

#endif

class ProtectLayer {
    uint8_t         m_node_id;  // TODO not set yet
    uint8_t         m_msg_buffer[MAX_MSG_SIZE];

    AES             m_aes;
    AEShash         m_hash;
    AESMAC          m_mac;
    Crypto          m_crypto;
    KeyDistrib      m_keydistrib;

    CTP             m_ctp;

#ifdef __linux__
    uTeslaMaster    *m_utesla;
    int             m_slave_fd;
#else
#ifdef ENABLE_UTESLA
    uTeslaClient    m_utesla;
#endif
#endif

public:
#ifdef __linux__
    ProtectLayer(std::string &slave_path, std::string &key_file);    // throws runtime_error if there is a problem with key file
    virtual ~ProtectLayer();

    uint8_t broadcastMessage(uint8_t *buffer, uint8_t size);    // TODO not implemented yet
    uint8_t sendTo(msg_type_t msg_type, uint8_t receiver, uint8_t *buffer, uint8_t size); // TODO not implemented yet
#else

    /**
     * @brief ProtectLayer constructor
     * 
     */
    ProtectLayer();

    /**
     * @brief Send message to another node protected by encryption and MAC with pairwise key shared with node
     * 
     * @param msg_type  Type of the message
     * @param receiver  Receiver
     * @param buffer    Data to be sent
     * @param size      Size of the data
     * @return uint8_t  SUCCESS on success, FAIL on failure
     */
    uint8_t sendTo(msg_type_t msg_type, uint8_t receiver, uint8_t *buffer, uint8_t size);

    /**
     * @brief Send message to a CTP parent
     * 
     * @param msg_type  Type of the message
     * @param buffer    Data to be sent
     * @param size      Size of the data
     * @return uint8_t  SUCCESS on success, FAIL on failure
     */
    uint8_t sendCTP(msg_type_t msg_type, uint8_t *buffer, uint8_t size);

    /**
     * @brief Send data to BS. Method depends on msg_type - MSG_APP sends it directly in one hop, MSG_FORWARD uses CTP routing to forward a message.
     * 
     * @param msg_type  Type of the message. MSG_APP - send directly, MSG_FORWARD - send to CTP parent
     * @param buffer    Data to be sent
     * @param size      Size of the data
     * @return uint8_t  SUCCESS on success, FAIL on failure
     */
    uint8_t sendToBS(msg_type_t msg_type, uint8_t *buffer, uint8_t size);

    /**
     * @brief Forward message to BS through CTP parent without any modification
     * 
     * @param buffer    Data to be sent
     * @param size      Size of the data
     * @return uint8_t  SUCCESS on success, FAIL on failure
     */
    uint8_t forwardToBS(uint8_t *buffer, uint8_t size);

    /**
     * @brief Receive message intended for this node.
     * 
     * @param buffer        Received already decrypted message.
     * @param buff_size     Size of the buffer for received message
     * @param received_size Size of the received message
     * @param timeout       Time in milliseconds to wait for a message
     * @return uint8_t      SUCCESS on success, FAIL on failure or FORWARD if the message was intended for BS
     */
    uint8_t receive(uint8_t *buffer, uint8_t buff_size, uint8_t *received_size, uint16_t timeout);

    uint8_t discoverNeighbors();
#endif

    uint8_t startCTP();

    uint8_t receive(uint8_t *buffer, uint8_t buff_size, uint8_t *received_size);
};

#endif //  PROTECTLAYER_H