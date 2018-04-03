
--Don't define a graphics renderer! This is just a simple console application.

--Create folders required by the test cases
CreateFolder("Test")
CreateFolder("Test\\Maps")
CreateFolder("Test\\Models")

--Tell the engine where to find packages
folders = {
	BaseDirectory="Test"
}


-- ==============================================================
-- Test Case Stuff
note={to="Tove"}

-- Test encapsulated nodes
A={B={x="1"}}