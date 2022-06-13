/*
 * Copyright 2013-2022 Real Logic Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package uk.co.real_logic.sbe;

import org.junit.jupiter.api.Test;
import uk.co.real_logic.sbe.ir.Ir;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;

public class TargetCodeGeneratorTest
{
    @Test
    public void shouldThrowOnNoTargetLanguage()
    {
        assertThrows(IllegalArgumentException.class, () ->
            SbeTool.generate(mock(Ir.class), ".", "none"));
    }

    @Test
    public void shouldLoadAndInstantiateNonStandardTargetLanguage() throws Exception
    {
        final Ir ir = mock(Ir.class);
        final String outputDir = ".";

        SbeTool.generate(ir, outputDir, "uk.co.real_logic.sbe.TestTargetLanguage");

        verify(TestTargetLanguage.SINGLETON).generate();
        assertEquals(TestTargetLanguage.ir, ir);
        assertEquals(TestTargetLanguage.outputDir, outputDir);
    }
}
