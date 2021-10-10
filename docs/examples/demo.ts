/*
 * Here's an example of usage of the libmaxminddb package with TypeScript.
 *
 * Install:
 * │ npm install libmaxminddb
 * │ # or
 * │ yarn add libmaxminddb
 *
 * Output:
 * │ Selected locale: en
 * │
 * │ 50.114.1.1:
 * │ - Continent: NA / North America
 * │ - Country: US / United States
 * │
 * │ 1.2.3.4:
 * │ - Not found
 */

import {MmdbReader} from 'libmaxminddb';

async function main() {
    const locale: GeoIP2Locale = 'en'; // You can try: 'de', 'en', 'es', 'fr', 'ja', 'pt-BR', 'ru', 'zh-CN'
    console.log('Selected locale: ' + locale);

    // Open the database (may throw an error if the database does not exist)
    const mmdb = await MmdbReader.open<GeoIP2CountryEntry>({
        filename: 'GeoIP2-Country-Test.mmdb',
    });

    for (const ip of ['50.114.1.1', '1.2.3.4']) {
        // Lookup the entry of an IP (may throw an error if the database has been closed or if the IP is badly formatted)
        const {netmask, entry} = await mmdb.lookup(ip);

        console.log(`\n${ip}:`);
        if (!entry) {
            // entry is undefined: there are no results for this IP
            console.log('- Not found');
        } else {
            // entry is defined!
            console.log(`- Continent: ${entry.continent.code} / ${entry.continent.names[locale]}`);
            console.log(`- Country: ${entry.country.iso_code} / ${entry.country.names[locale]}`);
        }
    }
}

main().catch(console.error);

/*
 * Below is an example of a typescript definition for MaxMind's GeoIP2 entries.
 * Reference: https://dev.maxmind.com/geoip/docs/databases/city-and-country/?lang=en
 */

interface GeoIP2CountryEntry {
    continent: Geo2IPContinent;
    country: Geo2IPCountry;
    registered_country: Geo2IPCountry;
}

interface Geo2IPContinent {
    code: GeoIP2ContinentCode;
    names: GeoIP2String;
    geoname_id: number;
}

type GeoIP2ContinentCode = 'AF' | 'AN' | 'AS' | 'EU' | 'NA' | 'OC' | 'SA';

interface Geo2IPCountry {
    iso_code: GeoIP2CountryCode;
    names: GeoIP2String;
    geoname_id: number;
}

type GeoIP2CountryCode = string; // ISO 3166-1 alpha-2 codes

type GeoIP2Locale = 'de' | 'en' | 'es' | 'fr' | 'ja' | 'pt-BR' | 'ru' | 'zh-CN';

type GeoIP2String = {[locale in GeoIP2Locale]: string};
