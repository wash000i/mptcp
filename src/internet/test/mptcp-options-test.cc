

#include "ns3/test.h"
#include "ns3/socket-factory.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/simulator.h"
#include "ns3/simple-channel.h"
#include "ns3/simple-net-device.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/config.h"
#include "ns3/ipv4-static-routing.h"
#include "ns3/ipv4-list-routing.h"
#include "ns3/ipv6-static-routing.h"
#include "ns3/ipv6-list-routing.h"
#include "ns3/node.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/uinteger.h"
#include "ns3/log.h"

#include "ns3/ipv4-end-point.h"
#include "ns3/arp-l3-protocol.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/icmpv4-l4-protocol.h"
#include "ns3/icmpv6-l4-protocol.h"
#include "ns3/udp-l4-protocol.h"
#include "ns3/tcp-l4-protocol.h"

#include "ns3/core-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/mp-tcp-socket-factory-impl.h"

#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/tcp-newreno.h"
#include "ns3/point-to-point-module.h"
#include "ns3/pcap-file.h"
#include "ns3/tcp-option-mptcp.h"
//#include "ns3/point-to-point-channel.h"
#include <string>

NS_LOG_COMPONENT_DEFINE ("MpTcpOptionsTestSuite");

using namespace ns3;


template<class T>
class TcpOptionMpTcpTestCase : public TestCase
{
public:
    TcpOptionMpTcpTestCase(Ptr<T> configuredOption,std::string desc) : TestCase(desc)
    {
        NS_LOG_FUNCTION(this);
        m_option = configuredOption;
    }

    virtual ~TcpOptionMpTcpTestCase()
    {
        NS_LOG_FUNCTION(this);
    }

    virtual void TestSerialize(void)
    {
        NS_LOG_INFO( "option.GetSerializedSize ():" << m_option->GetSerializedSize () );
        m_buffer.AddAtStart ( m_option->GetSerializedSize ());
        m_option->Serialize( m_buffer.Begin() );


    };

    virtual void TestDeserialize(void)
    {
        T option;
        Buffer::Iterator start = m_buffer.Begin ();
        uint8_t kind = start.ReadU8 ();



        NS_TEST_EXPECT_MSG_EQ (kind, TcpOption::MPTCP, "Option number does not match MPTCP sequence number");


        uint32_t read = option.Deserialize( start );

        NS_TEST_EXPECT_MSG_EQ ( read, option.GetSerializedSize(), "PcapDiff(file, file) must always be false");

        bool res= (*m_option == option);
        NS_TEST_EXPECT_MSG_EQ ( res,true, "Option loaded after serializing/deserializing are not equal. you should investigate ");
    };


    virtual void DoRun(void)
    {
        TestSerialize();
        TestDeserialize();
    }

protected:
    Ptr<T> m_option;
    Buffer m_buffer;
};

/**
this tests MPTCP (de)serializing options by generating pcap files and
comparing them to reference pcap files.
*/
#if 0
class TcpOptionMpTcpCapableTestCase : public TcpOptionMpTcpTestCase<TcpOptionMpTcpCapable>
{
public:

    // TestSerialize/ TestDeserialize
  TcpOptionMpTcpCapableTestCase();
  virtual ~TcpOptionMpTcpCapableTestCase();

  virtual void DoSetup(void);
  virtual void DoRun(void);
  virtual void DoTeardown (void);

};


TcpOptionMpTcpCapableTestCase::TcpOptionMpTcpCapableTestCase() :
    TestCase("mptcp-option-mpcapable")
{
    NS_LOG_FUNCTION(this);
}


TcpOptionMpTcpCapableTestCase::~TcpOptionMpTcpCapableTestCase()
{
    NS_LOG_FUNCTION(this);
}


void
TcpOptionMpTcpCapableTestCase::DoSetup(void)
{
    NS_LOG_INFO("Setup of MPTCP options test");
}


void
TcpOptionMpTcpCapableTestCase::DoRun(void)
{
    // Generate TcpHeader
//    Ipv4Header ipHeader;
//
//  TcpHeader h;
//  h.SetFlags( TcpHeader::SYN );
//  h.SetSequenceNumber(SequenceNumber32());
//  h.SetAckNumber(SequenceNumber32());
//  h.SetSourcePort(1);
//  h.SetDestinationPort(2);
//  h.SetWindowSize(10);

    ///////////////////////////////////////////////////
    /////    TestSerialize
    /////
    Buffer m_buffer;
//    Ptr<TcpOptionMpTcpCapable> option = Create<TcpOptionMpTcpCapable>();
    TcpOptionMpTcpCapable option, option2;
    option.SetSenderKey(1);
    option.SetRemoteKey(2);

    NS_LOG_INFO( "option.GetSerializedSize ():" << option.GetSerializedSize () );
    m_buffer.AddAtStart ( option.GetSerializedSize ());

    option.Serialize ( m_buffer.Begin () );

//    h.Serialize( buffer.Begin() );
//    TcpOptionTS opt;

    Buffer::Iterator start = m_buffer.Begin ();
    uint8_t kind = start.ReadU8 ();

    NS_TEST_EXPECT_MSG_EQ (kind, TcpOption::MPTCP, "Different kind found");

//    option2.Deserialize (start);
    uint32_t read = option2.Deserialize( start );

    NS_TEST_EXPECT_MSG_EQ ( read, option.GetSerializedSize(), "PcapDiff(file, file) must always be false");
    NS_TEST_EXPECT_MSG_EQ ( option.GetLocalKey(), option2.GetLocalKey(), "Keys should be identic");

//    PcapFile
//    uint32_t sec(0), usec(0);
//    static bool 	Diff (std::string const &f1, std::string const &f2, uint32_t &sec, uint32_t &usec, uint32_t snapLen=SNAPLEN_DEFAULT
//    bool diff = ns3::PcapFile::Diff (std::string const &f1, std::string const &f2, sec, usec, SNAPLEN_DEFAULT);
//    std::string fullPath = CreateDataDirFilename();
//    CreateTempDirFilename

// 0x1e0c0081ec98f2ec7e8654d0
//    NS_TEST_EXPECT_MSG_EQ (diff, false, "PcapDiff(file, file) must always be false");
//    TcpOptionMpTcpCapable

    /////////////////////////////////////////////////
    /// DSS check
//    0x1e142005b7d964e16860e7ac0000000100186118

}


void
TcpOptionMpTcpCapableTestCase::DoTeardown(void)
{

}
#endif // 0

static class TcpOptionMpTcpTestSuite : public TestSuite
{
public:
 TcpOptionMpTcpTestSuite ()
 : TestSuite ("mptcp-option", UNIT)
 {
//    for (uint8_t i=0; i< 40; i += 10)
//    {

        ////////////////////////////////////////////////
        //// MP CAPABLE
        ////
        Ptr<TcpOptionMpTcpCapable> mpc = CreateObject<TcpOptionMpTcpCapable>(),
                mpc2 = CreateObject<TcpOptionMpTcpCapable>();
        mpc->SetRemoteKey(42);
        mpc->SetSenderKey(232323);
        AddTestCase(
            new TcpOptionMpTcpTestCase<TcpOptionMpTcpCapable> (mpc,"MP_CAPABLE with Sender & Peer keys both set"),
            QUICK
            );

       mpc2->SetSenderKey(3);
        AddTestCase(
            new TcpOptionMpTcpTestCase<TcpOptionMpTcpCapable> (mpc2,"MP_CAPABLE with only sender Key set"),
            QUICK
            );



        ////////////////////////////////////////////////
        //// MP PRIORITY
        ////
        Ptr<TcpOptionMpTcpChangePriority> prio = CreateObject<TcpOptionMpTcpChangePriority>() ,
                prio2 = CreateObject<TcpOptionMpTcpChangePriority>();

        prio->SetAddressId(3);
        AddTestCase(
            new TcpOptionMpTcpTestCase<TcpOptionMpTcpChangePriority> (prio,"Change priority for a different address"),
            QUICK
            );



        prio->SetBackupFlag(true);
        AddTestCase(
            new TcpOptionMpTcpTestCase<TcpOptionMpTcpChangePriority> (prio2,"Change priority for current address with backup flag ons"),
            QUICK
            );

        ////////////////////////////////////////////////
        //// MP REMOVE_ADDRESS
        ////
        // TODO generate random/real  addresses  with IPv4 helper ?
        Ptr<TcpOptionMpTcpRemoveAddress> rem = CreateObject<TcpOptionMpTcpRemoveAddress>();
         for (uint8_t i=0; i<15; ++i)
         {
            Ptr<TcpOptionMpTcpRemoveAddress> rem2 = CreateObject<TcpOptionMpTcpRemoveAddress>();

            rem->AddAddressId(i);
            rem2->AddAddressId(i);

            AddTestCase(
                new TcpOptionMpTcpTestCase<TcpOptionMpTcpRemoveAddress> (rem,"With X addresses"),
                QUICK
                );

            AddTestCase(
                new TcpOptionMpTcpTestCase<TcpOptionMpTcpRemoveAddress> (rem2,"With 1 address"),
                QUICK
                );

     }


        ////////////////////////////////////////////////
        //// MP ADD_ADDRESS
        ////
        Ptr<TcpOptionMpTcpAddAddress> add = CreateObject<TcpOptionMpTcpAddAddress>();
        add->SetAddress( InetSocketAddress( "123.24.23.32"), 8 );


//        add2.SetAddress( InetSocketAddress( "ffe2::1"), 4 );

        AddTestCase(
                new TcpOptionMpTcpTestCase<TcpOptionMpTcpAddAddress> (add,"AddAddress IPv4"),
                QUICK
                );


        ////////////////////////////////////////////////
        //// MP
        ////
        Ptr<TcpOptionMpTcpDSN> dsn = CreateObject<TcpOptionMpTcpDSN>(),
                dsn2 = CreateObject<TcpOptionMpTcpDSN>(),
                dsn3 = CreateObject<TcpOptionMpTcpDSN>();
        MpTcpMapping mapping;
        mapping.Configure( SequenceNumber32(54),32);
        mapping.MapToSubflowSeqNumber( SequenceNumber32(40));


        dsn->SetMapping(mapping);


        AddTestCase(
                new TcpOptionMpTcpTestCase<TcpOptionMpTcpDSN> (dsn,"DSN mapping only"),
                QUICK
                );

        dsn2->SetDataAck(3210);
        AddTestCase(
                new TcpOptionMpTcpTestCase<TcpOptionMpTcpDSN> (dsn2,"DataAck only"),
                QUICK
                );

        dsn->SetDataAck(45000);
        AddTestCase(
                new TcpOptionMpTcpTestCase<TcpOptionMpTcpDSN> (dsn,"DataAck + DSN mapping"),
                QUICK
                );


        ////////////////////////////////////////////////
        //// MP TcpOptionMpTcpJoinInitialSyn
        ////
        Ptr<TcpOptionMpTcpJoinInitialSyn> syn = CreateObject<TcpOptionMpTcpJoinInitialSyn>(),
                    syn2 = CreateObject<TcpOptionMpTcpJoinInitialSyn>();
        syn->SetAddressId(4);
        syn->SetPeerToken(5323);
        AddTestCase(
                new TcpOptionMpTcpTestCase<TcpOptionMpTcpJoinInitialSyn> ( syn, "MP_JOIN Syn"),
                QUICK
                );



        ////////////////////////////////////////////////
        //// MP TcpOptionMpTcpJoinSynReceived
        ////
        Ptr<TcpOptionMpTcpJoinSynReceived> jsr = CreateObject<TcpOptionMpTcpJoinSynReceived>(),
            jsr2 = CreateObject<TcpOptionMpTcpJoinSynReceived>();
        jsr->SetAddressId(4);
        jsr->SetTruncatedHmac( 522323 );
        AddTestCase(
                new TcpOptionMpTcpTestCase<TcpOptionMpTcpJoinSynReceived> ( jsr, "MP_JOIN Syn Received"),
                QUICK
                );



        ////////////////////////////////////////////////
        //// MP TcpOptionMpTcpJoinSynReceived
        ////
        Ptr<TcpOptionMpTcpJoinSynAckReceived> jsar = CreateObject<TcpOptionMpTcpJoinSynAckReceived>();
        uint8_t hmac[20] = {3,0};
        jsar->SetHmac( hmac  );
        AddTestCase(
                new TcpOptionMpTcpTestCase<TcpOptionMpTcpJoinSynAckReceived> ( jsar, "MP_JOIN SynAck Received"),
                QUICK
                );

//     Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
//
//     for (uint32_t i=0; i<1000; ++i)
//     {
//         AddTestCase (new TcpOptionTSTestCase ("Testing serialization of random "
//         "values for timestamp",
//         x->GetInteger (),
//         x->GetInteger ()), TestCase::QUICK);
//     }

 }




} g_TcpOptionTestSuite;