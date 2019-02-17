-- ------------------------
-- Subject: FLP 2018/2019
-- Project: simplify-bkg
-- Date:    2019-02-16
-- Author:  Tomas Aubrecht
-- Login:   xaubre02
-- ------------------------

-- Imports
import System.Environment -- getArgs
import System.IO          -- handles
import qualified Data.Set as Set -- TOD: what is qualified?
import Text.Printf
import Data.List  -- intercalate, map, lines
import Data.Char -- isAlpha, isUpper

-- ---------- CUSTOM RECORDS -----------

-- A -> alpha
data Rule = Rule
  {
    left :: Char,    -- A: nonterminal
    right :: [Char]  -- alpha: in (N u T)*
  } deriving (Show, Eq, Ord)

-- G = (N, T, S, P)
data Grammar = Grammar
  {
    nonterminals :: Set.Set Char,  -- N: a finite set of nonterminals
    terminals :: Set.Set Char,     -- T: a finite set of terminals
    startSymbol :: Char,           -- S: the start symbol
    rules :: Set.Set Rule          -- P: a finite set of rewrite rules
  }


-- ---------- PRINT FUNCTIONS ----------

-- print a set of characters
printCharSet list = putStrLn (intersperse ',' (Set.toList (list)))

-- print a single rule
printRule :: Rule -> IO()
printRule rule = printf "%c->%s\n" (left rule) (right rule)

-- print a set of rules
printRuleSet :: Set.Set Rule -> IO()
printRuleSet rules = mapM_ printRule (Set.toList(rules))

-- print a grammar
printGrammar :: Grammar -> IO()
printGrammar grammar = do
  printCharSet (nonterminals grammar)  -- print nonterminals
  printCharSet (terminals grammar)     -- print terminals
  printf "%c\n" (startSymbol grammar)  -- print start symbol
  printRuleSet (rules grammar)         -- print rules


-- ---------- PARSE FUNCTIONS ----------

-- remove spaces from a string
removeSpaces :: String -> String
removeSpaces [] = []
removeSpaces (' ':xs) = removeSpaces (xs)
removeSpaces (x:xs) = x : removeSpaces xs

-- split a string into separate parts by a comma
splitStr :: String -> [String]
splitStr string = split string []
  where
    split [] t = [t]
    split (x:xs) t = if x == ','
      then (t:split xs [])
      else split xs (t ++ [x])

-- check whether all elements in a list are unique - the list is a set
isSet :: [String] -> Bool
isSet = all ( (==) 1 . length) . sg
  where
    sg :: Ord a => [a] -> [[a]]
    sg = group . sort

-- parse nonterminals
parseNonterminals :: String -> Set.Set Char
parseNonterminals string = do
  let parts = splitStr (removeSpaces string)
  -- each part has to be a valid unique nonterminal symbol
  if isSet parts && all isNonterminal parts
    then Set.fromList (map head parts)
    else error "Nonterminals are not valid! Has to be a set of letters A-Z."
    where
      -- check whether the string is a nonterminal
      isNonterminal :: String -> Bool
      isNonterminal string = if (length string) == 1 && isAlpha (head string) && isUpper (head string)
        then True
        else False

-- parse terminals
parseTerminals :: String -> Set.Set Char
parseTerminals string = do
  let parts = splitStr (removeSpaces string)
  -- each part has to be a valid unique nonterminal symbol
  if isSet parts && all isTerminal parts
    then Set.fromList (map head parts)
    else error "Terminals are not valid! Has to be a set of letters a-z."
    where
      -- check whether the string is a terminal
      isTerminal :: String -> Bool
      isTerminal string = if (length string) == 1 && isAlpha (head string) && isLower (head string)
        then True
        else False

-- parse a start symbol from a string
parseStartSymbol :: String -> Set.Set Char -> Char
parseStartSymbol string nonterminals =
  -- start symbol has to be one of the nonterminals
  if elem (head string) nonterminals
    then (head string)
    else error "Start symbol has to be one of the nonterminals!"

-- parse rewrite rules
parseRules :: [String] -> [Char] -> [Char] -> Set.Set Rule
parseRules rules nonterms terms =
  Set.fromList (mapM_ (parseRule nonterms terms) rules)
  where
--        parseRule :: String -> Set.Set Char -> Set.Set Char -> IO()
    parseRule nonterms terms rule = do
      let str = removeSpaces rule
      -- NONTERM -> alpha, where alpha in (NONTERMS u TERMS)*
      if (length str) >= 4 && elem (str !! 0) nonterms && str !! 1 == '-' && str !! 2 == '>' && ((isOK (drop 3 str) nonterms terms) || (drop 3 str) == "#")
        then Rule (str !! 0) (drop 3 str)
        else error ("'" ++ str ++ "' is an invalid rule!")
        where
          --isOK :: String -> Bool
          isOK rest nonterms terms = all (isValid nonterms terms) rest
          isValid nonterms terms char = elem char nonterms || elem char terms


-- parse input and return a grammar
--parseInput :: [String] -> Grammar
parseInput lines = do
  if (length lines) < 4
    then error "Invalid grammar on input!"  -- a grammar has to be a quaternion
    else do
      let nonterms = parseNonterminals (lines !! 0)
      let terms = parseTerminals (lines !! 1)
      --print (Set.toList nonterms)
      --print (Set.toList terms)
      --printf "%c\n" (parseStartSymbol (lines !! 2) nonterms)
      printRuleSet (parseRules (drop 3 lines) (Set.toList nonterms) (Set.toList terms))
      --printRuleSet (parseRules (drop 3 lines)) -- get only the lines with rules


-- ----------- MAIN FUNCTION -----------

main = do
  -- program arguments
  args <- getArgs
  -- mandatory argument not provided
  if (length args) == 0
    then error "Too few arguments!"
    -- too many arguments provided
    else if (length args > 2)
      then error "Too many arguments!"
      else do
        -- get input
        input <- if (length args) == 2
          then readFile (head (tail args))
          else hGetContents stdin

        -- first argument
        case(head args) of
          "-i" -> parseInput (lines input)
          "-1" -> putStrLn "argument -1"
          "-2" -> putStrLn "argument -2"
          otherwise -> error ("Unknown argument: " ++ (head args))

--        let r1 = Rule 'S' ['#']
--        let r2 = Rule 'S' ['a']

        --printList (Set.fromList ['a', 'b', 'c'])
--        let g = Grammar (Set.fromList ['S', 'V']) (Set.fromList ['a', 'b', 'c', 'c']) 'S' (Set.fromList [r1, r2, r1])
--        printGrammar g
