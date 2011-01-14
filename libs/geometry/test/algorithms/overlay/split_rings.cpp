// Boost.Geometry (aka GGL, Generic Geometry Library) test file
//
// Copyright Barend Gehrels 2007-2009, Geodan, Amsterdam, the Netherlands
// Copyright Bruno Lalande 2008, 2009
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <boost/detail/iomanip.hpp>
#include <string>

#include <geometry_test_common.hpp>

#include <boost/geometry/algorithms/detail/overlay/split_rings.hpp>
#include <boost/geometry/algorithms/area.hpp>

#include <boost/geometry/strategies/strategies.hpp>

#include <boost/geometry/extensions/gis/io/wkt/read_wkt.hpp>
#include <boost/geometry/extensions/gis/io/wkt/write_wkt.hpp>


#if defined(TEST_WITH_SVG)
#  include <boost/geometry/extensions/io/svg/svg_mapper.hpp>
#endif


struct test_split_rings
{
    template <typename Geometry>
    static void apply(std::string const& id,
            Geometry const& geometry,
            int expected_ring_count,
            double precision)
    {

        typedef typename bg::ring_type<Geometry>::type ring_type;
        std::vector<ring_type> rings;
        bg::split_rings(geometry, rings);

        BOOST_CHECK_EQUAL(rings.size(), expected_ring_count);


#if defined(TEST_WITH_SVG)
        {
            std::ostringstream filename;
            filename << "split_rings_" << id << ".svg";

            std::ofstream svg(filename.str().c_str());

            bg::svg_mapper<typename bg::point_type<Geometry>::type> mapper(svg, 500, 500);
            mapper.add(geometry);

            mapper.map(geometry, "fill:rgb(255,255,128);stroke:rgb(0,0,0);stroke-width:1");

            BOOST_FOREACH(ring_type const& ring, rings)
            {
                std::string style = "opacity:0.6;fill:rgb";
                std::string color = bg::area(ring) > 0 ? "(255,0,0)" : "(0,0,255)";
                mapper.map(ring, style + color + ";stroke:rgb(128,128,128);stroke-width:3");
            }
        }
#endif
    }
};


template <typename Geometry>
void test_geometry(std::string const& caseid,
            std::string const& wkt,
            int expected_ring_count, double precision = 0.001)
{
    if (wkt.empty())
    {
        return;
    }

    Geometry geometry;
    bg::read_wkt(wkt, geometry);

    test_split_rings::apply(caseid, geometry, expected_ring_count, precision);
}


#if ! defined(GEOMETRY_TEST_MULTI)

template <typename P>
void test_self_all()
{
    typedef bg::model::polygon<P> polygon;

    // Simplex

    test_geometry<polygon>("1", "POLYGON((0 0,0 10,10 10,5 2,5 8,10 0,0 0))", 2);


    // // Many loops (e.g. as result from buffering)
    test_geometry<polygon>("many_loops",
            "POLYGON((1 3,0 9,9 5,1 7,9 8,2 5,10 10,9 2,1 3))", 3);


    // From "mill" (flower)
    test_geometry<polygon>("2",
        "POLYGON((5 6.10557,3.64223 8.82111,4.28284 8.71716,2.28284 6.71716,2.17889 7.35777,4.89443 6,2.17889 4.64223,2.28284 5.28284,4.28284 3.28284,3.62547 3.14045,5 6.10557))",
        5);

    test_geometry<polygon>("3",
        "POLYGON((0.6 10.4,9.4 10.4,9.4 -0.4,7.59 -0.4,4.59 5.33667,4.25383 5.6797,2.67526 4.89042,4.28284 3.28284,3.62547 3.14045,5.15 7.20587,5.15 -0.4,0.6 -0.4,0.6 10.4))",
        4);

    test_geometry<polygon>("4",
        "POLYGON((0.6 10.4,9.4 10.4,9.4 -0.4,4.59 -0.4,4.59 5.5,4.70431 5.22003,3.93716 6.00284,5 7.06569,6.06284 6.00284,5.29569 5.22003,5.41 5.5,5.41 0.01,5.01 0.41,5.25 0.41,4.85 0.01,4.85 7.20587,6.37453 3.14045,5.71716 3.28284,7.71716 5.28284,7.82111 4.64223,5.10557 6,7.82111 7.35777,7.71716 6.71716,5.71716 8.71716,6.35777 8.82111,5 6.10557,3.64223 8.82111,4.28284 8.71716,2.28284 6.71716,2.17889 7.35777,4.89443 6,2.17889 4.64223,2.28284 5.28284,4.28284 3.28284,3.62547 3.14045,5.15 7.20587,5.15 -0.4,0.6 -0.4,0.6 10.4))",
        21);


    // Split at end-point
    test_geometry<polygon>("end1", "POLYGON((0 -1,0 10,10 10,10 0,-1 0,0 -1))", 2);
    test_geometry<polygon>("end2", "POLYGON((0 -1,0 10,11 10,11 11,10 11,10 0,-1 0,0 -1))", 3);
    test_geometry<polygon>("end3", "POLYGON((0 -1,0 10,11 10,11 12,12 12,12 11,10 11,10 0,-1 0,0 -1))", 4);

    // Common boundaries
    test_geometry<polygon>("common1", "POLYGON((0 0,0 10,10 10,10 0,15 0,15 5,10 5,10 0,0 0))", 2);


    /*
    // From us counties
    test_geometry<polygon>("us_counties_1",
        "POLYGON((-111.5261116995073 41.22069282150765,-111.5217628285303 41.22035416591237,-111.5169320292328 41.22267065617934,-111.5202664326763 41.22008222445458,-111.524706358225 41.21912313459525,-111.5275767689257 41.2191200646373,-111.5313115885788 41.21977799763464,-111.5361319416122 41.22230930953956,-111.5390149025386 41.22669307111289,-111.5389640868764 41.22665820699549,-111.5405314519348 41.226478917027,-111.5432483336679 41.22674131537675,-111.5420873870024 41.22669699246593,-111.5468899132279 41.22714783689431,-111.5460245791811 41.22698484329734,-111.549259355884 41.2274477430645,-111.5577408755315 41.22811342877968,-111.5617324131657 41.22965779857636,-111.5622074202574 41.22994756325099,-111.5670818828246 41.22909494401993,-111.5692718200805 41.22891062885189,-111.5728540853548 41.22873442382645,-111.5760492668632 41.22941962741943,-111.5791498048043 41.22837886418323,-111.5837423095737 41.22837646600842,-111.5873509885199 41.22912252997033,-111.5905502098084 41.23037015657303,-111.5905738976172 41.23038614536871,-111.5911014306057 41.23028197627109,-111.5941112302391 41.22824505355918,-111.595808135602 41.22727811489068,-111.5980905931322 41.22548836332129,-111.5989075933465 41.22473873279626,-111.6007963250705 41.22266022435337,-111.6036899443978 41.22086457915817,-111.6058137732627 41.22012887375856,-111.602749721482 41.22196865310281,-111.5997971061567 41.22806209012482,-111.6002070563618 41.23129419775559,-111.6028708561713 41.23528460733029,-111.5994206515041 41.23211047917744,-111.5970021250484 41.22731367405874,-111.5968553199938 41.2213333698604,-111.5978240860405 41.218735705151,-111.59872799008 41.21674091429518,-111.5990118775156 41.21586922673351,-111.5993569133788 41.21346252288525,-111.601620124863 41.20717717448136,-111.6036896381354 41.20413147570312,-111.6066477579892 41.20036520970371,-111.6170938638088 41.19792503997269,-111.6171211409054 41.197912672181,-111.6174792281175 41.19763333099935,-111.6154497071748 41.19990210883104,-111.6145469690724 41.20214793299209,-111.6143872042365 41.20466380077419,-111.6140900270553 41.2012073328748,-111.6150859490031 41.19660529794054,-111.6218436555485 41.19061554473284,-111.6263257242001 41.18915337689279,-111.6371251993805 41.18909914015737,-111.6373680096377 41.18885567474617,-111.6383949129559 41.18739026040299,-111.6411123401382 41.18431339866869,-111.6444453559743 41.18263938395823,-111.6475502757163 41.18131111472079,-111.6480824344513 41.18111660627603,-111.6487261483388 41.18018217910811,-111.6541286582274 41.17582409853807,-111.6607502293818 41.17606808683036,-111.6639928385305 41.17750001069842,-111.6627289991998 41.17881191178137,-111.6627289991998 41.16984763275211,-111.6723147297616 41.17250652017602,-111.6740201607502 41.17050691221176,-111.6809028142628 41.17126673355348,-111.6809650818884 41.17123939654464,-111.6853833520725 41.17029389383428,-111.6898658836949 41.17133338502266,-111.6902209084905 41.17152318556664,-111.6904607105552 41.17152361200778,-111.6937608878567 41.171379538936,-111.6956197940083 41.17208616938441,-111.6993728847585 41.17040839085778,-111.7047507179242 41.17328962730216,-111.705138917194 41.1733253907057,-111.7102826252236 41.17325145150028,-111.7134998796559 41.17582525522811,-111.713857740997 41.1760577731046,-111.7143713164526 41.17626863893947,-111.714718580845 41.17638849963699,-111.7161627293466 41.17641647553787,-111.7235652490785 41.17698889260424,-111.7252956636535 41.18063938630633,-111.7255467374265 41.18070780591805,-111.7276362413481 41.18341443465751,-111.7281195035541 41.18379477216856,-111.7289633137073 41.18400466618191,-111.7322463245943 41.18537474159142,-111.7350417499302 41.18787350954477,-111.7400241492817 41.18920498371762,-111.7403897806728 41.18992392192225,-111.7405871404544 41.18993880963902,-111.7419062185388 41.19156105303389,-111.7441017621743 41.19149230781282,-111.74633522639 41.19326425318707,-111.7521804511058 41.19568179191514,-111.7548256119847 41.2000988714596,-111.7562439764081 41.20369244649578,-111.756385520958 41.20741568356885,-111.7560620666084 41.21002180147129,-111.7557493627381 41.2115435666562,-111.7556682922401 41.21250032230501,-111.7549762228438 41.2083729918014,-111.753021260936 41.20652061558319,-111.7508964302413 41.20557195114677,-111.7490502562344 41.20743627992862,-111.7538043672131 41.20092160586289,-111.7599851908795 41.19974075159288,-111.7535482000425 41.20277936096064,-111.7771999505038 41.20664084862795,-111.7822946483749 41.20868575255317,-111.7890479847428 41.20857546302415,-111.7834053984034 41.21113545825469,-111.8070916837301 41.21164475084303,-111.7983239620671 41.20690941545636,-111.7989057748419 41.20685382060309,-111.7988970680348 41.20685567247974,-111.8003950034205 41.20645434489455,-111.8015510337817 41.20521459774467,-111.8040078302751 41.20352268624269,-111.8051400156104 41.20305119804074,-111.805500030827 41.2024512874073,-111.8087760206539 41.19898729003837,-111.8127632488366 41.19571430853862,-111.8168246456107 41.19449127428283,-111.8226375152909 41.19565019698728,-111.8231921576817 41.19623717584509,-111.8268842679184 41.19390830631116,-111.8294386474025 41.19326999358571,-111.8281081895357 41.1923520714658,-111.83461841217 41.19040911228597,-111.8363820197726 41.18976234820737,-111.8389082690123 41.18928774535946,-111.8407278566805 41.18914288072003,-111.8414848277101 41.18915262958935,-111.8446813084015 41.18866243446734,-111.845959886583 41.18861802620197,-111.8434859428467 41.18901816674433,-111.8524610560402 41.1882713958049,-111.8481595765179 41.18832631814151,-111.8496570501088 41.18779673144824,-111.8533671749084 41.18768855682465,-111.8555901658855 41.18738820608631,-111.8545856366929 41.18543601585669,-111.8575805827953 41.18393299249777,-111.8615394981653 41.18172167621498,-111.8609477242236 41.1754400940934,-111.8606336522941 41.17523901959252,-111.8627856637925 41.16997723323662,-111.8635235797656 41.16904436111794,-111.8633266432236 41.16866453458245,-111.8616523096336 41.16843313106372,-111.8616216864947 41.16843907484764,-111.8576102899319 41.16714259614227,-111.8547179142898 41.16646709783558,-111.849004376199 41.16243082584917,-111.8507419858166 41.16392775774389,-111.8449330541267 41.15938516033442,-111.8453252871628 41.14671969870953,-111.8475090520856 41.14347927334031,-111.8495994494861 41.14184555885641,-111.8555300241724 41.12935093455723,-111.8539570692924 41.12913464596177,-111.8600529353453 41.12732099986341,-111.8628893201463 41.12732099986341,-111.8661516016343 41.12766178921265,-111.8717554038871 41.12781992480323,-111.8727125056091 41.12781941462108,-111.8744250029675 41.12734726013537,-111.8884146775109 41.12740408919985,-111.8910337085167 41.12670722950872,-111.8905605241137 41.12684570910798,-111.9013989090858 41.12571131225988,-111.902772044475 41.12619651923274,-111.9129115072118 41.12403191483529,-111.9147507512154 41.12495153622086,-111.9201178874858 41.12566715447374,-111.9240696750495 41.12855499923182,-111.9276546757843 41.12501918886934,-111.9355598663484 41.12798289437448,-111.9479782385554 41.12689279975358,-111.9529681915247 41.12994221534668,-111.9568859680716 41.13081283226165,-111.9574421207222 41.13136898494051,-111.9640025656762 41.1310956332098,-111.9657332834527 41.1401588859141,-111.9717215703218 41.14239547503718,-111.9730675011477 41.14266466120236,-111.9787329018218 41.14336183402796,-111.9883596218439 41.14364222393152,-111.9931363926252 41.14354196764271,-111.9994051904948 41.14363978973041,-112.0021307818896 41.14345808374524,-112.019489139395 41.14254400074554,-112.0239643318703 41.14234399927284,-112.026875 41.142344,-112.027575 41.142344,-112.0287262996005 41.142444,-112.029675 41.142444,-112.0285706847393 41.14250516265326,-112.0334643259497 41.14254400007785,-112.037275 41.142544,-112.038460765726 41.14254315085275,-112.0431214055661 41.14232416334449,-112.045375 41.142443"
        ",-112.045875 41.142443,-112.046183 41.142443,-112.046675 41.142443,-112.04701 41.142443,-112.047675 41.142443,-112.048675 41.142443,-112.050775 41.142443,-112.051675 41.142443,-112.0540827641383 41.14234300009903,-112.0612488438362 41.14234300000304,-112.0635234354713 41.14223960918194,-112.0770714181921 41.14244181784342,-112.083276 41.142343,-112.084776 41.142343,-112.0848417657806 41.14234299978541,-112.0924417865876 41.142443,-112.100276 41.142443,-112.103976 41.142443,-112.104114857411 41.14244299996745,-112.1076037688054 41.1425399140815,-112.112793190197 41.14242459354599,-112.1135036272676 41.14254300000501,-112.1226270227682 41.14254300006021,-112.1324270190691 41.142643,-112.1421638167791 41.142643,-112.1506273882427 41.14262237725859,-112.1576209575547 41.16131792925686,-112.152476 41.142743,-112.1733149590181 41.14274300002015,-112.1756526514941 41.14285431862275,-112.1721656863206 41.14330717334865,-112.1790628146085 41.14280884335523,-112.1808690864646 41.1427879609488,-112.1876051371955 41.1429370321104,-112.1984510116649 41.14057456440421,-112.1985386297479 41.14057190901539,-112.2482545857727 41.12928342466068,-112.2585216921568 41.1262568687031,-112.2815023152398 41.11964490348107,-112.3289786112679 41.10733319877986,-112.3533969705625 41.10142824273057,-112.5251560608342 41.05848586571712,-112.4535385157666 41.13192877978023,-112.3829356229766 41.20387761333999,-112.3628222295217 41.22454031797921,-112.3307981858544 41.25696262304565,-112.2578598211629 41.33099951523954,-112.2419961323004 41.34673900002596,-112.201955 41.34674,-112.198854 41.34674,-112.198567 41.34674,-112.197535 41.34674,-112.196755 41.34674,-112.183092 41.34674,-112.182661 41.34674,-112.174135 41.34674,-112.171521 41.34674,-112.164398 41.34674,-112.163765 41.34674,-112.162149 41.34674,-112.160616 41.34674,-112.152812 41.34674,-112.137305 41.34674,-112.133199 41.34674,-112.132112 41.34674,-112.1321083653075 41.34674,-112.1261408632321 41.34735552370944,-112.1210375425564 41.34723915345325,-112.0986867218297 41.34703960146247,-112.0991128873171 41.34703432373578,-112.096676 41.34703999999999,-112.0826884841567 41.34693962919125,-112.0727519042998 41.34683715821147,-112.0659830006501 41.34633201618576,-112.043791 41.346241,-112.038076 41.346241,-112.037476 41.346241,-112.033576 41.346241,-112.032963751752 41.346241,-112.0320859322845 41.34720047709238,-112.0293729847019 41.35158293065583,-112.0225987768525 41.35384099993897,-112.012945949278 41.35384099993896,-112.0051264048585 41.35982065155388,-111.9999931863867 41.35955048219633,-111.9985054002757 41.36093908247315,-111.9886378853145 41.36437975025613,-111.982438239708 41.36699931881362,-111.9768823000155 41.37105942855347,-111.9678461715639 41.3739866250941,-111.964824595067 41.37468391195694,-111.9684577169769 41.3568526391571,-111.9703144346862 41.35837247542384,-111.9717680562695 41.36002125916402,-111.9721806067458 41.36052336335063,-111.9722701353691 41.36056857086819,-111.9756933090284 41.36361139189865,-111.9764686134888 41.3653518489586,-111.9774815061067 41.36631107832983,-111.9784353725122 41.3673490439311,-111.9818608355456 41.36917725269873,-111.9855534354769 41.3760117250587,-111.9850442656137 41.37863404701749,-111.9854221363925 41.37887044147006,-111.977148428119 41.39494967066619,-111.9823464481754 41.39046084615109,-111.9775668170106 41.39635867866395,-111.9772955142989 41.39712343090701,-111.9777832268621 41.39396396792152,-111.9780769108747 41.39453595193084,-111.9779510967016 41.39732368111444,-111.9769673928205 41.40507237141038,-111.9776418373984 41.4063310929224,-111.979249745182 41.40756511192825,-111.9805546890196 41.41174800617959,-111.9821969091629 41.41376647554345,-111.9822412429523 41.41771857136868,-111.9841029081325 41.42395514822981,-111.9758267412905 41.42973332568962,-111.9744143892868 41.43003915215967,-111.974044426735 41.43044369722418,-111.9724487099302 41.43127793250221,-111.9714554951038 41.43218647543674,-111.9705674410934 41.4326839955389,-111.969173938818 41.43455463604666,-111.9691313074541 41.43514229114804,-111.9698532625346 41.44083524341373,-111.9563035057409 41.44387691758892,-111.9528519001313 41.4425035997071,-111.948862688548 41.43996812957263,-111.9436557129119 41.43464132813732,-111.944571811535 41.43108183151026,-111.9453783570838 41.43429662278894,-111.9453292374341 41.43423156460978,-111.9487047416977 41.4370350518674,-111.949293219852 41.43728152821434,-111.946617863753 41.43651233182759,-111.9427973525344 41.43430728368595,-111.9423776809036 41.43367777659645,-111.9419291943176 41.4334303851785,-111.9407777658475 41.43217660903539,-111.9381879763191 41.43031650193143,-111.9382493895579 41.43040002340472,-111.9372011434928 41.42981718018637,-111.934117584732 41.42860635849613,-111.9324487007684 41.4262717701259,-111.9302590931362 41.42612441754618,-111.925590477184 41.4240869309046,-111.9246983708718 41.42340851775217,-111.9216328785657 41.42175106235152,-111.9201304574531 41.41957427158692,-111.9218350345417 41.42105808569211,-111.9265579698133 41.4224674871054,-111.9269374253964 41.42243152261963,-111.92904005315 41.4216476704315,-111.9263529265228 41.42314117163985,-111.9215977648415 41.42317758175992,-111.9196428552148 41.42261319977101,-111.9169146898828 41.42287472206833,-111.9134215054519 41.42473907134696,-111.9126447259132 41.42469479648941,-111.9124490246428 41.42499795177306,-111.9092387641912 41.42755224569281,-111.9052380937632 41.42897848938426,-111.9002628533518 41.42946382895553,-111.8971561379816 41.42889450986716,-111.8960249780191 41.4283771327491,-111.8940193677521 41.42850175875044,-111.8960340763695 41.42767896956915,-111.9024176724123 41.42135665487622,-111.9015750043214 41.42528897612509,-111.8998000296643 41.4285722194028,-111.8966205495849 41.43183063874344,-111.8944406431162 41.43324514380939,-111.8926252959783 41.43414332950123,-111.8916804311569 41.43450204532494,-111.8813854987459 41.43984949127724,-111.880095916456 41.43458669143437,-111.8830979471463 41.43487808596424,-111.8730501071175 41.43221499037259,-111.8646587191902 41.42862291364479,-111.8681104012376 41.41281394982138,-111.8676066232081 41.41192879397556,-111.8662685472666 41.41047503332864,-111.8648360831269 41.40927056113344,-111.8617555367589 41.40987864206158,-111.8571815473483 41.40797796402953,-111.8529378794857 41.40290666028592,-111.8525219677778 41.40333283159944,-111.8527417149485 41.40333718389994,-111.8501119188309 41.40364548567232,-111.8450021686117 41.40306327006346,-111.843627880848 41.40234608708582,-111.8435199095021 41.40231074220579,-111.844881585803 41.40265278843807,-111.8349256853031 41.39920519979233,-111.832436319031 41.39890094762616,-111.8320872552582 41.39798934323525,-111.8361009415533 41.3978803505358,-111.8321021292179 41.40300459287702,-111.8241291143356 41.40167512219369,-111.8217725690825 41.40211299539512,-111.8212955423522 41.40200922486019,-111.8171546776915 41.40329458953078,-111.8125316083106 41.39846505320347,-111.8124294708664 41.39842288523761,-111.8124264284935 41.3984236578345,-111.8108857608745 41.39858652417184"
        ",-111.809365213452 41.39987879371605,-111.8042760434507 41.4011908453571,-111.8020686543937 41.40053613590567,-111.8039046363505 41.40017458184678,-111.8042635478739 41.39999258459775,-111.7909954737669 41.40089644205416,-111.7929629310693 41.40139132981063,-111.7846743021944 41.40036870539331,-111.7827866126724 41.39571814524774,-111.7815214169703 41.39299863043483,-111.7828920971421 41.39327334027993,-111.7748509130635 41.38743730378992,-111.7752165761997 41.38718480775791,-111.7721697223399 41.38604483298051,-111.7750983614573 41.386555270279,-111.7639414583505 41.38192065911581,-111.761963781468 41.3800412809659,-111.7613108864837 41.37912231691754,-111.7612258742607 41.37910304832786,-111.7591299416527 41.37968085372221,-111.7577907259937 41.37944029328051,-111.7558579035919 41.38022277149841,-111.7512688505652 41.38095536128881,-111.7482174462501 41.37990334283357,-111.7486646160195 41.37996229584262,-111.7501551896196 41.37882469173843,-111.7454941763277 41.38178624311301,-111.7335398767194 41.38465825647904,-111.7344375425085 41.38458725363097,-111.7324914280349 41.38482975996588,-111.7308102203539 41.38492274099217,-111.730258 41.364938,-111.7400445096964 41.37466081033997,-111.7367972666407 41.37827413406988,-111.7369761626924 41.37865937323883,-111.7373499817572 41.38191202877975,-111.7362382343994 41.3896817282629,-111.7318062031051 41.39385726751244,-111.730934876677 41.39610325892399,-111.7281898573264 41.39852698503418,-111.7232992768295 41.40141614738422,-111.7216699706312 41.40155945218662,-111.7235231380814 41.40068052773415,-111.7265750913976 41.3963025736558,-111.7201029932023 41.40005616511778,-111.7177896512755 41.40746098221877,-111.7158070848994 41.41038399195119,-111.715655176932 41.41047706608663,-111.715983156649 41.40463400375688,-111.7204406800612 41.40810070196405,-111.7274933391879 41.41205246687121,-111.7285125917203 41.41467440623902,-111.728522980037 41.41344595317905,-111.7322942449122 41.41795959302958,-111.731896106543 41.42560633754115,-111.7320332087593 41.42926118743524,-111.729957646899 41.43302822141643,-111.7118843501114 41.43114694815002,-111.7140310900979 41.43355552193208,-111.7078739864605 41.43654866026967,-111.6998524797112 41.43271471504839,-111.6986652926243 41.42986102526688,-111.6975120881016 41.42897853603878,-111.6971210984819 41.4282934699863,-111.6961606079428 41.42773075204737,-111.6959864719628 41.42776692740181,-111.6933497690836 41.42989872972967,-111.6930019315523 41.42994586089555,-111.6917153721713 41.43152040923766,-111.6902295574764 41.4326187150197,-111.6857899530101 41.43448702217745,-111.6836120286734 41.43709471482509,-111.6771247652154 41.43723637943522,-111.6779983979918 41.43725697173675,-111.6749495172627 41.43734007411281,-111.6746735498744 41.43741591954125,-111.6720209667629 41.43783594325005,-111.6727109972352 41.43770166372526,-111.6553038799185 41.42796846134614,-111.6559813580971 41.42929086121254,-111.6584331526525 41.4317013458235,-111.6584370945208 41.43170379247909,-111.6526596626538 41.42834717102206,-111.6520987713805 41.42542774863863,-111.651155399945 41.42497098966823,-111.6485038542844 41.42069082966789,-111.6484473142877 41.42048207616851,-111.6481701857523 41.42030165647017,-111.6458111338422 41.41860283437335,-111.6435250708396 41.41556473317775,-111.6434325770253 41.41553749590582,-111.6387074295628 41.41091893071695,-111.6382542875749 41.40697282822121,-111.6430587268327 41.41423110967988,-111.6474946531616 41.41493287990986,-111.6519130312862 41.41196173789954,-111.6533453458041 41.4089750382549,-111.6534670630606 41.40818567942468,-111.6532180676668 41.41233729755218,-111.650141671316 41.41784707703802,-111.6506362105335 41.41721786169201,-111.6483458444573 41.42038505559527,-111.6399883913355 41.42456782419585,-111.6387619333563 41.42531302770971,-111.6372313167869 41.42610278062988,-111.6388772156545 41.42503724999391,-111.6241185051481 41.42565671005583,-111.6292989198989 41.42747881108829,-111.6330212257612 41.42632935426848,-111.6300888540781 41.42874135696512,-111.6258525865511 41.43045665694785,-111.6200937473438 41.43292197700307,-111.6163818162207 41.43227133372919,-111.6139121279047 41.43210192316946,-111.6127778034731 41.431778181874,-111.6100998879303 41.43186873969825,-111.6033371287108 41.42772766687219,-111.6022619251007 41.41977026042356,-111.6042208823911 41.42642276689722,-111.6042547368996 41.42641106290917,-111.608203778722 41.42866129535266,-111.6088030126456 41.42877501302603,-111.6100720883581 41.428785889551,-111.6085308515393 41.42932400636524,-111.606750545072 41.42926095100312,-111.6050156683234 41.42987323575301,-111.6032457428657 41.43066814395039,-111.5990673748855 41.43148472494484,-111.5935002234159 41.43042488220517,-111.5930272861467 41.42998515359408,-111.5930267097009 41.42998503477877,-111.5926383972181 41.42952847560235,-111.5893656740488 41.42939120933852,-111.5879879513229 41.42897465490946,-111.5877655061638 41.42920784693975,-111.5872128087414 41.42998364520915,-111.583805331358 41.43402279679681,-111.5850015607415 41.43257430449172,-111.5682595644039 41.43689127289402,-111.5655580307463 41.43431733658391,-111.5648140723382 41.43437789786282,-111.5588494686844 41.43147389314375,-111.5627733442355 41.43390475163831,-111.5467595603931 41.43097085352397,-111.5435231017314 41.4216909235525,-111.5442217676426 41.42410058028637,-111.5429581915997 41.4198629141784,-111.5415905096045 41.41916883276773,-111.5377466703981 41.42118674271469,-111.4999002636551 41.40754598864975,-111.49992321854 41.41992376877793,-111.5043942337116 41.42316548043323,-111.5074667581468 41.42367669767896,-111.5018577201973 41.42339139933106,-111.498492694593 41.42223736811422,-111.4908347022589 41.41568090530068,-111.4908130094445 41.41544015588016,-111.4899750552739 41.41493216608176,-111.4892746770538 41.41472074332899,-111.4877789754887 41.41479252991904,-111.4780235955907 41.41446855024663,-111.4758343817126 41.4101305233553,-111.4667945916543 41.408008495426,-111.4649303818979 41.39743051733797,-111.4670499026164 41.38880238826368,-111.4675403497742 41.39238946277144,-111.4682693256192 41.3896798556904,-111.468223034458 41.38866896494368,-111.4679524737185 41.3873696044556,-111.467909790308 41.38511421668602,-111.4689252899892 41.3893194192861,-111.4719613794916 41.39260783946975,-111.4773850453548 41.3939272250177,-111.4755047817366 41.39401042247292,-111.4730700536672 41.3936966334367,-111.4707125540257 41.39313997077441,-111.4681148394472 41.39206078401419,-111.4673020106218 41.39150164420799,-111.4650634653196 41.39118921312598,-111.4624398349077 41.3906400254072,-111.4605400175199 41.38972232484358,-111.4570460693707 41.38909801417777,-111.4548503602481 41.38578941112745,-111.4481916270633 41.37903507093495,-111.4538905371506 41.37167946112217,-111.4523110338275 41.37562482145626,-111.453122061191 41.38030214182341,-111.4559274593289 41.38258137778414,-111.4578400905912 41.38326186498487,-111.4580092624499 41.38328010181085,-111.4545500072416 41.38310423553524,-111.453828693243 41.38286176547096,-111.4513917306495 41.38316888047494,-111.4490880680225 41.38286920566615,-111.4458883535083 41.38283562865421,-111.4410455091279 41.38056286130516,-111.4296540092564 41.37759174629597,-111.4330684620716 41.37199485958408,-111.4325288499295 41.37170115977816,-111.433423614824 41.37148481989143,-111.4327514468304 41.37229760229101,-111.4292876194388 41.37329041914894,-111.4279628046368 41.37382224950171,-111.4310504194527 41.37201570165918,-111.4220511206116 41.37680632625185,-111.4132007135549 41.36916475528064,-111.4132556937664 41.36821137521456,-111.410599947015 41.3655167694235,-111.4095467780071 41.35588239586497,-111.4153923235931 41.35080670253447,-111.4157574283901 41.3503345776012,-111.4141595503214 41.35370156318688,-111.4141624489772 41.35339760480722,-111.4115288588478 41.35121628241728"
        ",-111.4156461230987 41.34027082280225,-111.4156347336163 41.34011622708512,-111.4219171529141 41.33399691991721,-111.4230512496498 41.33351877639811,-111.4246536800136 41.33255158448824,-111.4278802645644 41.3316442458971,-111.4321841269485 41.3326384089827,-111.4310435307007 41.3328305819942,-111.4298921041398 41.33381070679388,-111.4325816436783 41.34640754412428,-111.4305938656625 41.34507358150004,-111.4288086603161 41.342542060306,-111.431481679049 41.34513109369425,-111.4287994278175 41.33815667720043,-111.4278232779227 41.33641772801344,-111.4265560012482 41.33289501968777,-111.4265560012482 41.32617628698502,-111.4272107382647 41.32560778902531,-111.4272178782421 41.32554523388917,-111.4261980734283 41.32225817088516,-111.4304919167739 41.31367048419434,-111.4363369977304 41.31257559725096,-111.4394237666417 41.31270635296883,-111.4394344156974 41.31270409565712,-111.441519161219 41.31236426658026,-111.4381960052264 41.31363247642526,-111.4343497992445 41.31844116068382,-111.4351634377084 41.3136073258965,-111.4398662896243 41.30910418220708,-111.4422468135143 41.30752683142005,-111.44538081134 41.30634785128562,-111.4477252742131 41.30622230681761,-111.4450982072919 41.30679223598104,-111.4415499070308 41.30926911751111,-111.440343520513 41.31155182354399,-111.4451062874178 41.31989072583871,-111.4414187617719 41.31986572481608,-111.4386063431991 41.31574156424257,-111.4372288340086 41.31425825843935,-111.4357467129998 41.3119401785306,-111.4339202494052 41.30822377235391,-111.4336625137004 41.30069611228576,-111.436193501451 41.29832538323529,-111.4342850140522 41.29308194363609,-111.4328391389641 41.29555271021763,-111.4325216530159 41.29665639935071,-111.4347508608123 41.30443260100356,-111.4360586780902 41.30564636670918,-111.4346792471093 41.30464063662748,-111.4254390049755 41.29528237788455,-111.4291302686912 41.29068338555521,-111.4261573074688 41.28463104839097,-111.4370267030589 41.27981638195083,-111.4375558321362 41.27935123578043,-111.4354383430839 41.27995632635503,-111.4416162807751 41.2758529929185,-111.4384076084416 41.27957295063146,-111.4377212864738 41.2850520602332,-111.4381740345049 41.28651100478594,-111.4384610963577 41.28700771059263,-111.4443864706288 41.29120163044556,-111.4392995187362 41.28993258176533,-111.437506253794 41.28634485567613,-111.4356263875061 41.28302577228677,-111.4349277183834 41.27937332380423,-111.4350242056712 41.26951553923891,-111.4419729572562 41.26823923756828,-111.443851402553 41.26628117225569,-111.4466503255125 41.26617791598309,-111.4466539930302 41.26617467737448,-111.4520015669708 41.26396940238351,-111.4535032224145 41.26442883740561,-111.4575335757568 41.26357415147228,-111.4483713214452 41.27007784433943,-111.4479650010794 41.2717768437342,-111.4480273600067 41.27313463677613,-111.4493445602483 41.27659822195652,-111.4478190730863 41.27422750187978,-111.4464117327744 41.26893674882813,-111.4496732569538 41.26159468063268,-111.4543300686382 41.25793771380997,-111.4571410161368 41.2577942766178,-111.4584177002777 41.25730159262817,-111.4553050787541 41.25932963474278,-111.4530081358515 41.26959930131982,-111.4508994156196 41.26371450013092,-111.4541957910179 41.25962837787136,-111.4533513082429 41.25606707336917,-111.4584511900491 41.24989054984832,-111.4611419330649 41.24901272751228,-111.4642296892219 41.2471079915566,-111.466302267792 41.24724208780359,-111.4612041454673 41.25525650795958,-111.4613628730978 41.25711686602586,-111.460012566547 41.25068020785515,-111.4631190491229 41.24557484651866,-111.4668960494423 41.24198842878425,-111.4691669157075 41.2417000374843,-111.4697812813245 41.24112614793989,-111.4746504843807 41.23477980181183,-111.4803977314971 41.23547461817721,-111.4807668379497 41.23549157614718,-111.4821598518453 41.235181287545,-111.4835147362251 41.23397825845257,-111.4913294700869 41.23144137067924,-111.4937928098947 41.23127714842143,-111.4853525806721 41.23734674935701,-111.4851730213113 41.24391252958296,-111.4868064098187 41.24667054274708,-111.4899998360328 41.24898263383229,-111.4827890866642 41.24415984037166,-111.4821185211798 41.23272415759732,-111.4852056387818 41.22855546079995,-111.4871367392634 41.22636620973306,-111.4877302368984 41.22602162842327,-111.4866371991571 41.22728810367443,-111.4862618064053 41.21244302667023,-111.5038439093175 41.21886594606829,-111.5086526752272 41.22023174933462,-111.5093913926803 41.21966687966173,-111.5156021372448 41.21908063181036,-111.5179945315531 41.21978213857661,-111.5200627063573 41.21974916010367,-111.5242768213902 41.22009310457656,-111.5261116995073 41.22069282150765))",
        22);
        */

}
#endif



#if ! defined(GEOMETRY_TEST_MULTI)
int test_main(int, char* [])
{
    test_self_all<bg::model::d2::point_xy<double> >();
    return 0;
}
#endif
