-- Cellular Automata Program
-- Written for CMU SCS Discrete Math Primer (15-051)
--
-- High level usage:
--   > simulate xor center 38
--   > simulate (rule 110) leftmost 76

-- a cell can be in one of two states
type Cell      = Bool
type Config    = [Cell]
-- LocalRule: (a cell) -> (left neighbor) -> (right neighbor) -> (new cell)
type LocalRule = Cell -> Cell -> Cell -> Cell

-- for instance, a simple rule is to take the xor of two neighbors
xor :: LocalRule
xor _ = (/=)

-- a more complex local rule xor's all three adjacent cells
xor_all :: LocalRule
xor_all cell left right = (cell /= left) /= right

-- useful lengths
width     = 77 :: Int -- does not include border or center
halfwidth = floor $ (fromIntegral width) / 2
half      = take halfwidth $ repeat False

-- some popular initial configurations
center, leftmost, rightmost, alternate :: Config
center    = half ++ [True] ++ half
leftmost  = [True] ++ half ++ half
rightmost = half ++ half ++ [True]
alternate = foldl (++) [] $ take halfwidth $ repeat [True, False]

-- global rule: update the entire configuration
-- this is done by synchronously applying the local rule to each cell
global :: LocalRule -> Config -> Config
global rule config =
  map (local config) [0..hi]
  where
    local config index =
      let len   = length config
          cell  = (!!) config index
          left  = (!!) config $ mod (index + len - 1) len
          right = (!!) config $ mod (index + 1) len
       in rule cell left right
    hi = length config - 1

-- format simulation outputs as blocks with pretty borders
blockify :: Config -> [Char]
blockify config =
  "│" ++ (map (\x -> if x then '█' else ' ') config) ++ "│"

frame :: [Char] -> [Char]
frame content =
  top ++ content ++ bottom
  where
    len (x:xs) = if x == '\n' then 0 else 1 + (len xs)
    horizontal = take (len content - 2) $ repeat '─'
    top        = "┌" ++ horizontal ++ "┐\n"
    bottom     = "\n└" ++ horizontal ++ "┘"

-- ultimate simulation wrapper for a given LocalRule, staring Config, and depth
simulate :: LocalRule -> Config -> Int -> IO ()
simulate rule initial depth =
  putStrLn $ frame $ run (global rule) initial depth
  where
    run rule initial 0 = blockify initial
    run rule initial n =
      (blockify initial) ++ "\n" ++ (run rule (rule initial) (n - 1))

-- make a LocalRule from its Wolfram code
rule :: Int -> LocalRule
rule n =
  \cell left right ->
    (map to_bool $ pad8 $ binary_expand n) !! (7 - cell_as_num cell left right)
  where
    to_num x = if x then 1 else 0
    to_bool  = not . (== 0)
    cell_as_num c l r =
      sum $ map (\(x,y) -> 2^x * y) $ zip [0..] $ map to_num [l, c, r]


-- some utility functions --
-- rounded log base 2 of an integer
log2 :: Int -> Int
log2 x = floor $ log (fromIntegral x) / log 2

-- expand, e.g. 12 -> [1, 1, 0, 0]
binary_expand :: Int -> [Int]
binary_expand n =
  bin_exp_aux n $ log2 n
  where
    bin_exp_aux x d
      | d < 0     = []
      | x >= 2^d  = 1 : bin_exp_aux (x - 2^d) (d - 1)
      | otherwise = 0 : bin_exp_aux x (d - 1)

-- force binary number to length 8, chopping off upper bits if needed
pad8 :: [Int] -> [Int]
pad8 num
  | length num <= 8 = pad ++ num
  | length num > 8  = drop len num
  where
    len = abs $ 8 - (length num)
    pad = take len $ repeat 0
