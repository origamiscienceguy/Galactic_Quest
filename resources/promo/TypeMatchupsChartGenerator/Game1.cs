using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using System.IO;

namespace TypeMatchupsChartGenerator
{
    public class Game1 : Game
    {
        private GraphicsDeviceManager _graphics;
        private SpriteBatch spriteBatch;

        // Define the types and damage values
        string[] types = {
            "Fire", "Water", "Grass", "Electric", "Ice", "Rock", "Psychic"
        };
        
        int[,] damageValues = {
            { 50, 25, 100, 50, 100, 25, 50 },
            { 100, 50, 25, 50, 50, 100, 50 },
            { 25, 100, 50, 50, 50, 100, 50 },
            { 50, 100, 25, 50, 50, 50, 50 },
            { 25, 50, 100, 50, 50, 100, 50 },
            { 100, 50, 50, 50, 50, 50, 50 },
            { 50, 50, 50, 50, 50, 50, 50 }
        };

        SpriteFont font;
        Texture2D blankTexture;
        RenderTarget2D renderTarget;
        Texture2D _texShipRedStripImg;
        Texture2D _texShipBlueStripImg;

        int gridSize = 64; // size of each cell
        int chartSize;
        Vector2 gridOrigin;

        public Game1()
        {
            _graphics = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";
            IsMouseVisible = true;
        }

        protected override void Initialize()
        {
            // TODO: Add your initialization logic here

            base.Initialize();
        }

        protected override void LoadContent()
        {
            spriteBatch = new SpriteBatch(GraphicsDevice);

            // Load font and create blank texture
            font = Content.Load<SpriteFont>("Arial"); // Ensure you have a font named "Arial" in your Content folder
            blankTexture = new Texture2D(GraphicsDevice, 1, 1);
            blankTexture.SetData(new[] { Color.White });

            // Load the sprite sheet texture
            _texShipRedStripImg = Content.Load<Texture2D>("lightShipsRed");
            _texShipBlueStripImg = Content.Load<Texture2D>("lightShipsBlue");

            // Calculate chart size and create render target
            chartSize = types.Length * gridSize;
            renderTarget = new RenderTarget2D(GraphicsDevice, chartSize, chartSize, false, SurfaceFormat.Color, DepthFormat.None);

            // Draw to the render target
            DrawToRenderTarget();
        }

        protected override void Update(GameTime gameTime)
        {
            if (GamePad.GetState(PlayerIndex.One).Buttons.Back == ButtonState.Pressed || Keyboard.GetState().IsKeyDown(Keys.Escape))
                Exit();

            // TODO: Add your update logic here

            base.Update(gameTime);
        }

        protected override void Draw(GameTime gameTime)
        {
            GraphicsDevice.Clear(Color.CornflowerBlue);

            // TODO: Add your drawing code here

            base.Draw(gameTime);
            Exit();
        }

        private void DrawToRenderTarget()
        {
            int gridSize = 64; // Assuming grid size is 64x64
            int labelSize = gridSize; // Ensure there's enough space for the labels
            int chartSizeWithLabels = types.Length * gridSize + labelSize; // Account for labels on all sides

            // Create a new RenderTarget2D with space for labels
            renderTarget = new RenderTarget2D(
                GraphicsDevice,
                chartSizeWithLabels,
                chartSizeWithLabels,
                false,
                SurfaceFormat.Color,
                DepthFormat.None
            );

            // Set the render target
            GraphicsDevice.SetRenderTarget(renderTarget);
            GraphicsDevice.Clear(Color.CornflowerBlue);

            spriteBatch.Begin();

            // Draw the grid cells
            for (int i = 0; i < types.Length; i++)
            {
                for (int j = 0; j < types.Length; j++)
                {
                    // Draw grid cells
                    Rectangle cellRect = new Rectangle(
                        i * gridSize + labelSize, // Offset for x-axis labels
                        j * gridSize + labelSize, // Offset for y-axis labels
                        gridSize,
                        gridSize
                    );
                    spriteBatch.Draw(blankTexture, cellRect, Color.LightGray);

                    // Draw damage value in the center of each cell
                    string damageText = damageValues[i, j].ToString();
                    Vector2 textSize = font.MeasureString(damageText);
                    Vector2 textPosition = new Vector2(
                        i * gridSize + labelSize + (gridSize - textSize.X) / 2,
                        j * gridSize + labelSize + (gridSize - textSize.Y) / 2
                    );
                    spriteBatch.DrawString(font, damageText, textPosition, Color.Black);

                    // Draw dotted lines
                    DrawDottedLine(spriteBatch, new Vector2(cellRect.Right, cellRect.Top), new Vector2(cellRect.Right, cellRect.Bottom), Color.Yellow);
                    DrawDottedLine(spriteBatch, new Vector2(cellRect.Left, cellRect.Bottom), new Vector2(cellRect.Right, cellRect.Bottom), Color.Yellow);
                }
            }

            // Draw the x and y axes types
            for (int i = 0; i < types.Length; i++)
            {
                // Calculate text positions for x-axis labels
                Vector2 xTypePosition = new Vector2(
                    i * gridSize + labelSize + (gridSize - font.MeasureString(types[i]).X) / 2,
                    0
                );

                // Ensure the x-axis label is within the bounds
                if (xTypePosition.X >= 0 && xTypePosition.X < chartSizeWithLabels && xTypePosition.Y >= 0 && xTypePosition.Y < labelSize)
                {
                    spriteBatch.DrawString(font, types[i], xTypePosition, Color.Black);


                    // Draw the 16x16 graphics from the vertical strip image
                    int graphicIndex = i; // Example index; adjust as needed
                    Rectangle graphicRect = new Rectangle(
                        0, // X position (vertical strip, so X is 0)
                        graphicIndex * 16, // Y position of the current tile in the vertical strip
                        16, // Width of each graphic
                        16  // Height of each graphic
                    );
                    Rectangle destinationRect = new Rectangle(
                        (int)xTypePosition.X + 32-9-4,
                        25+7,
                        16,
                        16
                    );
                    spriteBatch.Draw(_texShipRedStripImg, destinationRect, graphicRect, Color.White);

                }

                // Calculate text positions for y-axis labels
                Vector2 yTypePosition = new Vector2(
                    0,
                    i * gridSize + labelSize + (gridSize - font.MeasureString(types[i]).Y) / 2
                );

                // Ensure the y-axis label is within the bounds
                if (yTypePosition.X >= 0 && yTypePosition.X < labelSize && yTypePosition.Y >= 0 && yTypePosition.Y < chartSizeWithLabels)
                {
                    spriteBatch.DrawString(font, types[i], yTypePosition, Color.Black);
                }
            }

            spriteBatch.End();

            // Reset the render target
            GraphicsDevice.SetRenderTarget(null);

            // Scale up the image by 4x
            ScaleImage(4);
        }

        private void ScaleImage(int scaleFactor)
        {
            // Calculate the new size
            int newWidth = renderTarget.Width * scaleFactor;
            int newHeight = renderTarget.Height * scaleFactor;

            // Create a new render target for the scaled image
            RenderTarget2D scaledRenderTarget = new RenderTarget2D(
                GraphicsDevice,
                newWidth,
                newHeight,
                false,
                SurfaceFormat.Color,
                DepthFormat.None
            );

            // Set the new render target
            GraphicsDevice.SetRenderTarget(scaledRenderTarget);
            GraphicsDevice.Clear(Color.CornflowerBlue);

            spriteBatch.Begin();

            // Draw the scaled image
            spriteBatch.Draw(
                renderTarget,
                new Rectangle(0, 0, newWidth, newHeight),
                Color.White
            );

            spriteBatch.End();

            // Reset the render target
            GraphicsDevice.SetRenderTarget(null);

            // Optionally save the scaledRenderTarget to a file
            SaveRenderTargetAsPng(scaledRenderTarget, "type_matchup_chart.png");
        }

        // Helper function to draw dotted lines
        private void DrawDottedLine(SpriteBatch sB, Vector2 start, Vector2 end, Color color)
        {
            float distance = Vector2.Distance(start, end);
            float step = 10f; // Distance between dots
            Vector2 direction = Vector2.Normalize(end - start);

            for (float i = 0; i < distance; i += step * 2)
            {
                Vector2 dotPosition = start + direction * i;
                sB.Draw(blankTexture, dotPosition, null, color, 0f, Vector2.Zero, 1f, SpriteEffects.None, 0f);
            }
        }

        private void SaveRenderTargetAsPng(RenderTarget2D renderTarget, string fileName)
        {
            // Save the RenderTarget2D to a file (example with Texture2D.SaveAsPng)
            using (var stream = File.Create(fileName))
            {
                renderTarget.SaveAsPng(stream, renderTarget.Width, renderTarget.Height);
            }
        }
    }
}
