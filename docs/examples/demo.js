/*
 * Here's an example of usage of the libmaxminddb package with Javascript.
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

const {MmdbReader} = require('libmaxminddb');

async function main() {
    const locale = 'en'; // You can try: 'de', 'en', 'es', 'fr', 'ja', 'pt-BR', 'ru', 'zh-CN'
    console.log('Selected locale: ' + locale);

    // Open the database (may throw an error if the database does not exist)
    const mmdb = await MmdbReader.open({
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
