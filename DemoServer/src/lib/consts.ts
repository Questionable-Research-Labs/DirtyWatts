// Average NZ Load
// Found via taking medium of the last year of samples in the DB to two sig figs
/* SQL Sample
CREATE OR REPLACE FUNCTION sig_digits(n anyelement, digits int)
    RETURNS numeric
AS
$$
SELECT CASE
           WHEN n = 0 THEN 0
           ELSE round(n, digits - 1 - floor(log(abs(n)))::int)
           END
$$ LANGUAGE sql IMMUTABLE
                STRICT;

select sig_digits(CAST((
    PERCENTILE_CONT(0.5) WITHIN GROUP (ORDER BY GenerationSums.sum)
    ) as numeric), 5)
from (select sum(generation) from generation_levels where reading_timestamp > current_date - interval '1' year group by reading_timestamp) GenerationSums;
*/
// Should eventually auto-update with an API call
export const AVERAGE_NZ_LOAD_MW = 4700; // Updated 2023-04-11