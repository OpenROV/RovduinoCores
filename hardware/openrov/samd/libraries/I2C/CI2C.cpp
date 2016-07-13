#include "CI2C.h"

// --------------------
// DONE

CI2C::CI2C( SERCOM *s, uint8_t pinSDA, uint8_t pinSCL )
{
	m_pSercom					= s;
	m_pinSDA					= pinSDA;
	m_pinSCL					= pinSCL;
}

CI2C::~CI2C()
{
	
}

I2C::ERetCode CI2C::Enable( uint32_t baudRateIn, uint16_t optionsIn )
{
	// Initialize in master mode
	I2C::ERetCode ret = m_pSercom->InitMasterMode_I2C( baudRateIn, optionsIn );

	// Failed to initialize, either because of invalid baud rate or it is already enabled
	if( ret )
	{
		return ret;
	}

	m_pSercom->Enable_I2C();

	pinPeripheral( m_pinSDA, g_APinDescription[m_pinSDA].ulPinType );
	pinPeripheral( m_pinSCL, g_APinDescription[m_pinSCL].ulPinType );

	return I2C::ERetCode::SUCCESS;
}

I2C::ERetCode CI2C::Disable()
{
	m_pSercom->Disable_I2C();

	return I2C::ERetCode::SUCCESS;
}

bool CI2C::IsAvailable()
{
	// Check sercom enabled bit
	return m_pSercom->IsEnabled_I2C();
}

// Write operations
I2C::ERetCode CI2C::WriteByte( uint8_t slaveAddressIn, uint8_t dataIn )
{
	// Set up transfer
	m_transfer.slaveAddress = slaveAddressIn;
	m_transfer.buffer 		= m_pTransferBuffer;
	m_pTransferBuffer[ 0 ]	= dataIn;
	m_transfer.length		= 1;
	m_transfer.action		= I2C::EAction::WRITE;

	// Perform transfer
	return m_pSercom->PerformTransfer_I2C( &m_transfer );
}

I2C::ERetCode CI2C::WriteByte( uint8_t slaveAddressIn, uint8_t registerIn, uint8_t dataIn )
{
	// Set up transfer
	m_transfer.slaveAddress = slaveAddressIn;
	m_transfer.buffer 		= m_pTransferBuffer;
	m_pTransferBuffer[ 0 ]	= registerIn;
	m_pTransferBuffer[ 1 ]	= dataIn;
	m_transfer.length		= 2;
	m_transfer.action		= I2C::EAction::WRITE;

	// Perform transfer
	return m_pSercom->PerformTransfer_I2C( &m_transfer );
}

I2C::ERetCode CI2C::WriteWord( uint8_t slaveAddressIn, uint16_t dataIn )
{
	// Set up transfer
	m_transfer.slaveAddress = slaveAddressIn;
	m_transfer.buffer 		= m_pTransferBuffer;
	m_pTransferBuffer[ 0 ]	= dataIn >> 8;
	m_pTransferBuffer[ 1 ]	= dataIn & 0xFF;
	m_transfer.length		= 2;
	m_transfer.action		= I2C::EAction::WRITE;

	// Perform transfer
	return m_pSercom->PerformTransfer_I2C( &m_transfer );
}

I2C::ERetCode CI2C::WriteWord( uint8_t slaveAddressIn, uint8_t registerIn, uint16_t dataIn )
{
	// Set up transfer
	m_transfer.slaveAddress = slaveAddressIn;
	m_transfer.buffer 		= m_pTransferBuffer;
	m_pTransferBuffer[ 0 ]	= registerIn;
	m_pTransferBuffer[ 1 ]	= dataIn >> 8;
	m_pTransferBuffer[ 2 ]	= dataIn & 0xFF;
	m_transfer.length		= 3;
	m_transfer.action		= I2C::EAction::WRITE;

	// Perform transfer
	return m_pSercom->PerformTransfer_I2C( &m_transfer );
}

I2C::ERetCode CI2C::WriteBytes( uint8_t slaveAddressIn, uint8_t *dataIn, uint8_t numberBytesIn )
{
	// Set up transfer
	m_transfer.slaveAddress = slaveAddressIn;
	m_transfer.buffer 		= dataIn;				// Directly use the user's buffer
	m_transfer.length		= numberBytesIn;
	m_transfer.action		= I2C::EAction::WRITE;

	// Perform transfer
	return m_pSercom->PerformTransfer_I2C( &m_transfer );
}

I2C::ERetCode CI2C::WriteBytes( uint8_t slaveAddressIn, uint8_t registerIn, uint8_t *dataIn, uint8_t numberBytesIn )
{
	// Limit on input size
	if( numberBytesIn + 1 > kBufferSize_bytes )
	{
		return I2C::ERetCode::ERR_DENIED;
	}

	// Set up transfer
	m_transfer.slaveAddress = slaveAddressIn;
	m_transfer.buffer 		= m_pTransferBuffer;
	m_pTransferBuffer[ 0 ]	= registerIn;					
	memcpy( m_pTransferBuffer + 1, dataIn, numberBytesIn );	// Copy data from users buffer
	m_transfer.length		= numberBytesIn + 1;
	m_transfer.action		= I2C::EAction::WRITE;

	// Perform transfer
	return m_pSercom->PerformTransfer_I2C( &m_transfer );
}

I2C::ERetCode CI2C::WriteWords( uint8_t slaveAddressIn, uint16_t *dataIn, uint8_t numberWordsIn )
{
	// Set up transfer
	m_transfer.slaveAddress = slaveAddressIn;
	m_transfer.buffer 		= dataIn;				// Directly use the user's buffer
	m_transfer.length		= numberWordsIn * 2;
	m_transfer.action		= I2C::EAction::WRITE;

	// Perform transfer
	return m_pSercom->PerformTransfer_I2C( &m_transfer );
}

I2C::ERetCode CI2C::WriteWords( uint8_t slaveAddressIn, uint8_t registerIn, uint16_t *dataIn, uint8_t numberWordsIn )
{
	// Limit on input size
	if( numberWordsIn * 2 + 1 > kBufferSize_bytes )
	{
		return I2C::ERetCode::ERR_DENIED;
	}

	// Set up transfer
	m_transfer.slaveAddress = slaveAddressIn;
	m_transfer.buffer 		= m_pTransferBuffer;
	m_pTransferBuffer[ 0 ]	= registerIn;					
	memcpy( m_pTransferBuffer + 1, dataIn, numberWordsIn );	// Copy data from users buffer
	m_transfer.length		= numberWordsIn * 2 + 1;
	m_transfer.action		= I2C::EAction::WRITE;

	// Perform transfer
	return m_pSercom->PerformTransfer_I2C( &m_transfer );
}

// --------------------
// WIP

// Direct read operations (Uses user provided buffer)
I2C::ERetCode CI2C::ReadByte( uint8_t slaveAddressIn, uint8_t registerIn, uint8_t *dataOut )
{
	// BIG TODO: Rework with commands so we can write and read in a single transfer.

	// Set up register transfer
	m_transfer.slaveAddress = slaveAddressIn;
	m_transfer.buffer 		= dataOut;
	m_transfer.length		= 1;
	m_transfer.action		= I2C::EAction::WRITE;

	// Perform transfer
	return m_pSercom->PerformTransfer_I2C( &m_transfer );

	// Set up transfer
	m_transfer.slaveAddress = slaveAddressIn;
	m_transfer.buffer 		= dataOut;
	m_transfer.length		= 1;
	m_transfer.action		= I2C::EAction::READ;

	// Perform transfer
	return m_pSercom->PerformTransfer_I2C( &m_transfer );
}

I2C::ERetCode CI2C::ReadWord( uint8_t slaveAddressIn, uint8_t registerIn, uint16_t *dataOut )
{
	// Set up transfer
	m_transfer.slaveAddress = slaveAddressIn;
	m_transfer.buffer 		= dataOut;
	m_transfer.length		= 1;
	m_transfer.action		= I2C::EAction::READ;

	// Perform transfer
	return m_pSercom->PerformTransfer_I2C( &m_transfer );

	return I2C::ERetCode::SUCCESS;
}

I2C::ERetCode CI2C::ReadBytes( uint8_t slaveAddressIn, uint8_t registerIn, uint8_t *dataOut, uint8_t numberBytesIn )
{

	return I2C::ERetCode::SUCCESS;
}

I2C::ERetCode CI2C::ReadWords( uint8_t slaveAddressIn, uint8_t registerIn, uint16_t *dataOut, uint8_t numberWordsIn )
{

	return I2C::ERetCode::SUCCESS;
}

// Buffered read operations (Uses internal buffer)
I2C::ERetCode CI2C::ReadBytes_Buffered( uint8_t slaveAddressIn, uint8_t registerIn, uint8_t *dataOut, uint8_t numberBytesIn )
{

	return I2C::ERetCode::SUCCESS;
}

I2C::ERetCode CI2C::ReadWords_Buffered( uint8_t slaveAddressIn, uint8_t registerIn, uint16_t *dataOut, uint8_t numberWordsIn )
{

	return I2C::ERetCode::SUCCESS;
}

uint8_t CI2C::NextByte()
{

	return 0;
}

uint16_t CI2C::NextWord()
{
	return 0;
}

I2C::ERetCode CI2C::Scan()
{
	// TODO

	return I2C::ERetCode::SUCCESS;
}
